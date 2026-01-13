#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define PATH_SEPARATOR "\\"
#define getcwd _getcwd
#define chdir _chdir
#else
#define PATH_SEPARATOR "/"
#include <unistd.h>
#endif

typedef struct {
    char name[256];
    char type[64];  // "lib", "app", "tool"
    char sources[1024];
    char includes[512];
    char output[256];
    char depends[512];
    char path[1024];
} ProjectConfig;

typedef struct {
    ProjectConfig* projects;
    int count;
    int capacity;
} BuildSystem;

BuildSystem g_build = {0};

void error(const char* msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

void trim(char* str) {
    char* end;
    while (*str == ' ' || *str == '\t') str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) end--;
    *(end + 1) = '\0';
}

void parse_config_line(const char* line, ProjectConfig* config) {
    char buffer[1024];
    strncpy(buffer, line, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    trim(buffer);
    
    if (strlen(buffer) == 0 || buffer[0] == '#') return;
    
    char* equals = strchr(buffer, '=');
    if (!equals) return;
    
    *equals = '\0';
    char* key = buffer;
    char* value = equals + 1;
    trim(key);
    trim(value);
    
    if (strcmp(key, "name") == 0) {
        strncpy(config->name, value, sizeof(config->name) - 1);
    } else if (strcmp(key, "type") == 0) {
        strncpy(config->type, value, sizeof(config->type) - 1);
    } else if (strcmp(key, "sources") == 0) {
        strncpy(config->sources, value, sizeof(config->sources) - 1);
    } else if (strcmp(key, "includes") == 0) {
        strncpy(config->includes, value, sizeof(config->includes) - 1);
    } else if (strcmp(key, "output") == 0) {
        strncpy(config->output, value, sizeof(config->output) - 1);
    } else if (strcmp(key, "depends") == 0) {
        strncpy(config->depends, value, sizeof(config->depends) - 1);
    }
}

void load_project_config(const char* config_path, ProjectConfig* config) {
    memset(config, 0, sizeof(ProjectConfig));
    
    FILE* file = fopen(config_path, "r");
    if (!file) {
        fprintf(stderr, "Warning: Could not open config file %s\n", config_path);
        return;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        parse_config_line(line, config);
    }
    
    fclose(file);
}

void discover_projects(const char* base_path) {
    DIR* dir = opendir(base_path);
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s%s%s", base_path, PATH_SEPARATOR, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            char config_path[1024];
            snprintf(config_path, sizeof(config_path), "%s%s%s%sproject.ubuild", 
                    base_path, PATH_SEPARATOR, entry->d_name, PATH_SEPARATOR);
            
            if (g_build.count >= g_build.capacity) {
                g_build.capacity = g_build.capacity == 0 ? 16 : g_build.capacity * 2;
                g_build.projects = realloc(g_build.projects, g_build.capacity * sizeof(ProjectConfig));
            }
            
            load_project_config(config_path, &g_build.projects[g_build.count]);
            if (strlen(g_build.projects[g_build.count].name) > 0) {
                /* record the project's directory so we can build relative files */
                strncpy(g_build.projects[g_build.count].path, full_path, sizeof(g_build.projects[g_build.count].path)-1);
                g_build.projects[g_build.count].path[sizeof(g_build.projects[g_build.count].path)-1] = '\0';
                g_build.count++;
            }
            
            discover_projects(full_path);
        }
    }
    
    closedir(dir);
}

void build_project(ProjectConfig* config) {
    printf("Building: %s (%s)\n", config->name, config->type);

    /* save current working directory */
    char prev_cwd[1024];
    if (getcwd(prev_cwd, sizeof(prev_cwd)) == NULL) {
        perror("getcwd");
        error("Failed to get current directory");
    }

    /* change to project directory so relative sources work */
    if (strlen(config->path) > 0) {
        if (chdir(config->path) != 0) {
            perror("chdir");
            error("Failed to change to project directory");
        }
    }

    /* print current working directory for debugging */
    char curr_cwd[1024];
    if (getcwd(curr_cwd, sizeof(curr_cwd)) == NULL) {
        perror("getcwd");
    } else {
        printf("Building in: %s\n", curr_cwd);
    }

    /* Prepare include flags */
    char include_flags[1024] = "";
    if (strlen(config->includes) > 0) {
        char includes[1024];
        strcpy(includes, config->includes);
        char* t = strtok(includes, " ");
        while (t != NULL) {
            char include_flag[512];
            snprintf(include_flag, sizeof(include_flag), " -I%s", t);
            strcat(include_flags, include_flag);
            t = strtok(NULL, " ");
        }
    }

    /* Compile each source into its own .o file (replace extension with .o) */
    char sources[1024];
    strcpy(sources, config->sources);
    char* token = strtok(sources, " ");
    char obj_files[2048] = "";

    while (token != NULL) {
        /* derive object filename by removing existing extension */
        char obj_file_base[512];
        strncpy(obj_file_base, token, sizeof(obj_file_base) - 1);
        obj_file_base[sizeof(obj_file_base) - 1] = '\0';
        char* ext = strrchr(obj_file_base, '.');
        if (ext) *ext = '\0';

        char obj_path[512];
        snprintf(obj_path, sizeof(obj_path), "%s.o", obj_file_base);

        char compile_cmd[4096];
        snprintf(compile_cmd, sizeof(compile_cmd), "gcc -c -ffreestanding -fno-stack-protector -fPIC%s -o %s %s", include_flags, obj_path, token);
        printf("Compile command: %s\n", compile_cmd);
        int result = system(compile_cmd);
        if (result != 0) {
            /* restore cwd before exiting */
            chdir(prev_cwd);
            error("Compilation failed");
        }

        if (strlen(obj_files) > 0) strcat(obj_files, " ");
        strcat(obj_files, obj_path);

        token = strtok(NULL, " ");
    }

    if (strcmp(config->type, "lib") == 0) {
        char ar_cmd[4096];
        snprintf(ar_cmd, sizeof(ar_cmd), "ar rcs %s %s", config->output, obj_files);
        printf("Archive command: %s\n", ar_cmd);
        int result = system(ar_cmd);
        if (result != 0) {
            chdir(prev_cwd);
            error("Library creation failed");
        }
    } else if (strcmp(config->type, "app") == 0) {
        /* gather dependent libraries (use the recorded project paths) */
        char dep_libs[2048] = "";
        if (strlen(config->depends) > 0) {
            char depends_copy[512];
            strcpy(depends_copy, config->depends);
            char* d = strtok(depends_copy, " ");
            while (d != NULL) {
                /* find project by name */
                for (int i = 0; i < g_build.count; i++) {
                    if (strcmp(g_build.projects[i].name, d) == 0) {
                        char lib_path[1024];
                        snprintf(lib_path, sizeof(lib_path), "%s%s%s", g_build.projects[i].path, PATH_SEPARATOR, g_build.projects[i].output);
                        if (strlen(dep_libs) > 0) strcat(dep_libs, " ");
                        strcat(dep_libs, lib_path);
                        break;
                    }
                }
                d = strtok(NULL, " ");
            }
        }

        char link_cmd[8192];
        snprintf(link_cmd, sizeof(link_cmd), "gcc -nostdlib -ffreestanding -o %s %s %s -lgcc", config->output, obj_files, dep_libs);
        printf("Link command: %s\n", link_cmd);
        int result = system(link_cmd);
        if (result != 0) {
            chdir(prev_cwd);
            error("Linking failed");
        }
    }

    /* restore previous working directory */
    if (chdir(prev_cwd) != 0) {
        perror("chdir");
        error("Failed to restore working directory");
    }

    printf("Successfully built: %s\n", config->output);
}

int main(int argc, char* argv[]) {
    printf("Uefi.Build - Basic UEFI Build System\n");
    printf("=====================================\n\n");
    
    const char* build_dir = ".";
    if (argc > 1) {
        build_dir = argv[1];
    }
    
    printf("Discovering projects in: %s\n\n", build_dir);
    discover_projects(build_dir);
    
    if (g_build.count == 0) {
        error("No projects found");
    }
    
    printf("Found %d projects:\n", g_build.count);
    for (int i = 0; i < g_build.count; i++) {
        printf("  - %s (%s)\n", g_build.projects[i].name, g_build.projects[i].type);
    }
    printf("\n");
    
    /* Build libraries and tools first so apps can link against them */
    for (int i = 0; i < g_build.count; i++) {
        if (strcmp(g_build.projects[i].type, "lib") == 0 || strcmp(g_build.projects[i].type, "tool") == 0) {
            build_project(&g_build.projects[i]);
        }
    }

    /* Then build applications */
    for (int i = 0; i < g_build.count; i++) {
        if (strcmp(g_build.projects[i].type, "app") == 0) {
            build_project(&g_build.projects[i]);
        }
    }
    
    printf("\nBuild completed successfully!\n");
    
    free(g_build.projects);
    return 0;
}
