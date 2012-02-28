(*
 * To run this test, type "make" in this directory to build it and then run
 *   ./mycat some_file
 *
 * The test works as follows. The main process forks and its child execs
 * the ``openfile'' program, included here.
 *
 * The openfile program opens the required file and sends its descriptor
 * through a unix socket, which was inherited from the parent process.
 * Before and after sending the descriptor, a string is sent through the
 * socket, to ensure that send_fd is working correctly.
 *
 * Finally, the parent process waits for its child to exit and receives
 * the descriptor with recv_fd.
 *)

(* XXX http://camltest.inria.fr/pub/ml-archives/caml-list/2002/06/eab936a39747cc52dc8d139df0089338.fr.html *)
let int_of_file_descr (fd : Unix.file_descr) =
  let obj = Obj.repr fd in
  if Obj.is_int obj then string_of_int (Obj.obj obj)
  else invalid_arg "int_of_file_descr"

let read_and_print fd size =
  let buf = String.create size in
  ignore (Unix.read fd buf 0 size);
  print_endline buf

let exec_openfile fd path =
  let argsockfd = int_of_file_descr fd in
  try Unix.execv "./openfile" [| "openfile"; argsockfd; path |] with
  | Fd.Fd_error (s) -> raise (Failure s)

let get_descriptor_from_child pid fd =
  match Unix.waitpid [] pid with
  | (_, Unix.WEXITED status) ->
      if status = 0 then begin
        read_and_print fd 10;
        read_and_print (Fd.recv_fd ~conn:fd) 4096;
        read_and_print fd 10;
        fd
      end else
        raise (Failure "Child exited with non-zero status")
  | _ -> raise (Failure "Child terminated abnormally")

let my_open pathname =
  let fd0, fd1 = Unix.socketpair Unix.PF_UNIX Unix.SOCK_STREAM 0 in
  let fd = match Unix.fork () with
  | 0 ->
      Unix.close fd0;
      exec_openfile fd1 pathname
  | pid ->
      Unix.close fd1;
      get_descriptor_from_child pid fd0 in
  Unix.close fd0;
  fd

let _ =
  let file = Sys.argv.(1) in
  ignore (my_open file)
