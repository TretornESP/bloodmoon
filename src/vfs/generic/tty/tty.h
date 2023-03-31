struct tty_buffer {
    void * page;
    int size_filled;
    struct tty_buffer * next;
};

struct tty {
    int id;
    int mode;
    struct tty_buffer * input;
    struct tty_buffer * output;
    int signal;
    int com_port;
    void (*signal_handler)(int);
};