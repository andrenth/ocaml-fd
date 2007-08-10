let _ =
  match Unix.fork () with
  | 0 ->
      (let fd = Unix.openfile "/bin/ls" [Unix.O_RDONLY] 0o640 in
      try Fd.fexecve fd [| "/bin/ls"; "/" |] [| |] with
      Fd.Fd_error s -> print_endline s)
  | _ ->
      (let fd = Unix.openfile "/etc/passwd" [Unix.O_RDONLY] 0o640 in
      try Fd.fexecve fd [| "/etc/passwd"; |] [| |] with
      Fd.Fd_error s -> Printf.printf "fexecve failed: %s\n" s)
