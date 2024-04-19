

.global getpid 
getpid:
  mov w8, #0
  svc #0
  ret

.global uart_read
uart_read:
  mov w8, #1
  svc #0
  ret

.global uart_write
uart_write:
  mov w8, #2
  svc #0
  ret

.global exec
exec:
  mov w8, #3
  svc #0
  ret

.global fork
fork:
  mov w8, #4
  svc #0
  ret

.global sysexit
sysexit:
  mov w8, #5
  svc #0
  ret

.global sys_mbox_call
sys_mbox_call:
  mov w8, #6
  svc #0
  ret

.global kill
kill:
  mov w8, #7
  svc #0
  ret

.global my_open
my_open:
  mov w8, #11
  svc #0
  ret


.global close
close:
  mov w8, #12
  svc #0
  ret


.global write
write:
  mov w8, #13
  svc #0
  ret

.global read
read:
  mov w8, #14
  svc #0
  ret

.global mkdir
mkdir:
  mov w8, #15
  svc #0
  ret

.global mount
mount:
  mov w8, #16
  svc #0
  ret


.global chdir
chdir:
  mov w8, #17
  svc #0
  ret
