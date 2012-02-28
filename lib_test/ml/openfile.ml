(* XXX http://camltest.inria.fr/pub/ml-archives/caml-list/2002/06/eab936a39747cc52dc8d139df0089338.fr.html *)
let file_descr_of_int n =
  let fd : Unix.file_descr = Obj.magic (n : int) in
  try
    Unix.close (Unix.dup fd);
    fd
  with _ -> invalid_arg "file_descr_of_int"

let _ =
  let sockfd = file_descr_of_int (int_of_string Sys.argv.(1)) in
  let path = Sys.argv.(2) in
  let fd = Unix.openfile path [Unix.O_RDONLY] 0 in
  ignore (Unix.write sockfd "1234567890" 0 10);
  Fd.send_fd ~conn:sockfd ~fd:fd;
  ignore (Unix.write sockfd "0987654321" 0 10)
