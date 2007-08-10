(*
 * Copyright 2007 Digirati Informática, Serviços e Telecomunicações
 *
 * This file is distributed under the terms of version 2.1 of the GNU
 * Lesser General Public License. See the LICENSE file for details.
 *)

(** This module implements miscellaneous functions related to UNIX file
    descriptors. Currently, the functions listed below are implemented.

    - {!Fd.send_fd} and {!Fd.recv_fd}, which are respectively used to send
      and receive descriptors. Tipically, this functionality is used to
      allow a process to perform all the actions needed to acquire a
      descriptor, which is then sent to another process, which will then
      handle the data transfer operations on that descriptor.
    - {!Fd.fexecve}, which is used to execute a program specified via a
      file descriptor. *)

(** This exception is raised by {!Fd.send_fd} and {!Fd.recv_fd} when an error
    is encountered. The string contains the error message. *)
exception Fd_error of string

(** This function sends a descriptor over a Unix socket whose peer may
    be another process. The peer must receive the file descriptor with
    the function {!Fd.recv_fd} defined in this module.
  
    The descriptor is sent as a sized message, so the user application
    may use the connection to other communication needs.
  
    @param conn The connection through which the descriptor will be sent.
    @param fd The descriptor to be sent.
    @raise Fd_error This exception is raised on error. *)
val send_fd : conn:Unix.file_descr -> fd:Unix.file_descr -> unit

(** This function receives a descriptor sent by {!Fd.send_fd}.

    @param conn The connection through which the descriptor will be received. 
    @return The received file descriptor.
    @raise Fd_error This exception is raised on error. *)
val recv_fd : conn:Unix.file_descr -> Unix.file_descr

(** This function works like [Unix.execve], but the program to be executed,
    its first paramenter, is specified via a file descriptor. As is the case
    with the [Unix.execv*] functions, {!Fd.fexecve} never returns. If the
    call succeeds, the current process is substituted by the new one.
    
    @param fd The file descriptor corresponding to the program to be executed.
    @param args An array of arguments to be passed to the program.
    @param env The environment to the program.
    @raise Fd_error This exception is raised on error. *)
val fexecve : fd:Unix.file_descr -> args:string array -> env:string array ->
  unit
