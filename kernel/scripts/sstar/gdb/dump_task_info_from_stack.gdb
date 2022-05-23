target remote gdbserver:2331
file ./vmlinux
set print pretty
set height 0
hb schedule
command
printf "Schedule-out task: [%d] %s",((struct thread_info *)((unsigned int)$sp & ~((unsigned int)0x1FFF)))->task->pid,((struct thread_info *)((unsigned int)$sp & ~((unsigned int)0x1FFF)))->task->comm
c
end
