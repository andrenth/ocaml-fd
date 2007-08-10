(*
 * Copyright 2007 Digirati Informática, Serviços e Telecomunicações
 *
 * This file is distributed under the terms of version 2.1 of the GNU
 * Lesser General Public License. See the LICENSE file for details.
 *)

exception Fd_error of string

let _ = Callback.register_exception "Fd.Fd_error" (Fd_error "")

external send_fd : conn:Unix.file_descr -> fd:Unix.file_descr -> unit =
  "ocaml_send_fd"

external recv_fd : conn:Unix.file_descr -> Unix.file_descr =
  "ocaml_recv_fd"

external fexecve : fd:Unix.file_descr -> args:string array ->
  env:string array -> 'a = "ocaml_fexecve"
