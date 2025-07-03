#include <unistd.h>
#include <sys/wait.h>

void execute_pipe(char *cmd1[], char *cmd2[]) {
    int pipefd[2];
    pipe(pipefd);  // 创建管道

    if (fork() == 0) {  // 子进程1：执行左侧命令
        dup2(pipefd[1], STDOUT_FILENO);  // 重定向stdout到管道写端
        close(pipefd[0]);
        execvp(cmd1[0], cmd1);           // 替换为cmd1
    }

    if (fork() == 0) {  // 子进程2：执行右侧命令
        dup2(pipefd[0], STDIN_FILENO);   // 重定向stdin到管道读端
        close(pipefd[1]);
        execvp(cmd2[0], cmd2);            // 替换为cmd2
    }

    // 父进程关闭管道并等待子进程
    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);
}

// 示例：模拟 "ls | grep .c" 的操作
int main() {
    char *cmd1[] = {"ls", NULL};
    char *cmd2[] = {"grep", ".c", NULL};
    execute_pipe(cmd1, cmd2);
    return 0;
}