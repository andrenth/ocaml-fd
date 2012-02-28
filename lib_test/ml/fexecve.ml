let run_fexecve path args env =
  let fd = Unix.openfile path [Unix.O_RDONLY] 0o640 in
  try Fd.fexecve ~fd:fd ~args:args ~env:env with
  Fd.Fd_error s -> raise (Failure s)

let _ =
  match Unix.fork () with
  | 0 -> run_fexecve "/bin/ls" [| "/bin/ls"; "/" |] [| |]
  | _ -> run_fexecve "/etc/passwd" [| "/etc/passwd" |] [| |]
