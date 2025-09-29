#include "tarfs.h"
#include "../../../../kernel.h"

static inline uint64_t octal_to_int(const char *octal) {
    uint64_t integer = 0;
    while (*octal) {
        integer *= 8;
        integer += (*octal) - '0';
        octal++;
    }
    return integer;
}

static void ensure_parent_dirs(const char *path) {
    char tmp[strlen(path) + 1];
    strcpy(tmp, path);

    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            vfs_mkdir(tmp, 0777);
            *p = '/';
        }
    }
}

void mount_initrd(void) {
    vterm_print("Unpacking initrd...");

    if (!vfs_root)
        vfs_chroot(new_tmpfs());

    char *address = (char *)kernel->module->modules[0]->address;

    while (!memcmp(((ustar_header_t *)address)->ustar, "ustar", 5)) {
        const ustar_header_t *current_file = (ustar_header_t *)address;

        char full_path[strlen(current_file->name) + 2];
        full_path[0] = '/';
        strcpy(full_path + 1, current_file->name);

        size_t file_size = octal_to_int(current_file->file_size);

        if (current_file->type == USTAR_DIRTYPE) {
            ensure_parent_dirs(full_path);
            if (full_path[strlen(full_path) - 1] == '/')
                full_path[strlen(full_path) - 1] = '\0';

            int r = vfs_mkdir(full_path, octal_to_int(current_file->file_mode));
            if (r < 0) {
                vfs_node_t *n = vfs_open(full_path, VFS_READONLY);
                if (!n || !(n->flags & VFS_DIR)) {
                    kerror("Failed to create directory for initrd");
                    hcf();
                }
                if (n)
                    vfs_close(n);
            }

        } else if (current_file->type == USTAR_REGTYPE) {
            ensure_parent_dirs(full_path);
            if (vfs_create(full_path, 0777, VFS_FILE) < 0) {
                kerror("Failed to create file for initrd");
                hcf();
            }

            vfs_node_t *file = vfs_open(full_path, VFS_WRITEONLY);
            if (!file) {
                kerror("Failed to open file for initrd");
                hcf();
            }

            ssize_t write_size = vfs_write(file, address + 512, 0, file_size);
            if ((size_t)write_size != file_size) {
                kerror("Failed to write file for initrd");
                hcf();
            }

            file->size = file_size;
            vfs_close(file);

        } else if (current_file->type == USTAR_LNKTYPE) {
            ensure_parent_dirs(full_path);
            char *target = kmalloc(strlen(current_file->linked_file) + 2);
            if (!target) {
                kerror("kmalloc failed for symlink target");
                hcf();
            }
            target[0] = '/';
            strcpy(target + 1, current_file->linked_file);

            if (vfs_symlink(target, full_path) < 0) {
                kerror("Failed to create symlink for initrd");
                kfree(target);
                hcf();
            }

            kfree(target);
        }

        address += 512 + ((file_size + 511) / 512) * 512;
    }

    kok();
}
