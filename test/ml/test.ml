let test_send_fd () =
  let f = Unix.openfile "Makefile" [Unix.O_RDONLY] 640 in
  try Fd.send_fd f f with
  | Fd.Fd_error s -> Printf.printf "Fd.send_fd: %s\n" s

let _ = test_send_fd ()
