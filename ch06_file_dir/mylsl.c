#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_file_info(struct stat file_stat, char *file_name) {
    // Print file type and permissions
    // Take struct stat and file name as arguments
    printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");
    
    // Print number of links
    printf(" %ld", file_stat.st_nlink);
    
    // Print owner and group
    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group *gr = getgrgid(file_stat.st_gid);
    printf(" %s %s", pw->pw_name, gr->gr_name);
    
    // Print file size in bytes
    printf(" %ld", file_stat.st_size);
    
    // Print last modification date and time
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&file_stat.st_mtime)); // format the time into a readable string
    printf(" %s", time_str);
    
    // Print file name
    printf(" %s\n", file_name);
}

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *entry; // directory entry
    struct stat file_stat; // file status
    
    // Open the directory
    if (argc > 1) { // check if a directory is provided
        dir = opendir(argv[1]); 
    } else {
        dir = opendir("."); // open the current directory
    }
    
    // Check if the directory was opened successfully
    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }
    
    // Read directory entries
    while ((entry = readdir(dir)) != NULL) { // read the directory entries one by one
        if (stat(entry->d_name, &file_stat) == -1) { // get the file status
            perror("stat");
            continue;
        }
        print_file_info(file_stat, entry->d_name); // print the file information
    }
    
    closedir(dir); // close the directory
    return EXIT_SUCCESS; // return success
}
