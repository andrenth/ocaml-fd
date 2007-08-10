let test_send_fd =
  let f = Unix.openfile "Makefile" [Unix.O_RDONLY] 640 in
  try Fd.send_fd f f with
  | Unix.Unix_error (code, f, x) ->
      Printf.printf "%s: %s\n" f (Unix.error_message code)
  | Invalid_argument err ->
      Printf.printf "%s\n" err

let _ = test_send_fd
