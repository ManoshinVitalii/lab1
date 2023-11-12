#include <iostream>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/wait.h>
#include <fcntl.h>
#include <vector>

using namespace std;

int main() {
    string name;
    char c = '#';
    write(STDOUT_FILENO, "Enter the name of file: ", 24);
    while (c != '\n') {
        read(STDIN_FILENO, &c, sizeof(char));
        if (c != '\n') {
            name += c;
        }
    }

    int fd1[2], fd2[2];
    int pipe1 = pipe(fd1), pipe2 = pipe(fd2);
    if (pipe1 == -1 || pipe2 == -1) {
        write(STDERR_FILENO, "Err creating a pipe", 19);
        return 1;
    }

    int write1 = fd1[1], read1 = fd1[0];
    int write2 = fd2[1], read2 = fd2[0];

    pid_t pid = fork();
    if (pid == -1) {
        write(STDERR_FILENO, "Err creating a child process", 28);
        return 2;
    }
    if (pid == 0) {
        //child process
        close(write1);
        close(read2);
        int r1 = dup2(read1, STDIN_FILENO), w2 = dup2(write2, STDOUT_FILENO);
        if (r1 == -1 || w2 == -1) {
            write(STDERR_FILENO, "Err dup2", 8);
            return 3;
        }
        execl("child", "child", name.c_str(), NULL);

        write(STDERR_FILENO, "Err child process", 17);
        return 4;

    }
    if (pid > 0) {
        //parent process
        close(read1);
        close(write2);
        char c;
        while (read(STDIN_FILENO, &c, 1)) {
            write(write1, &c, sizeof(c));
        }
        close(write1);
        close(read2);
    }

    return 0;
}
