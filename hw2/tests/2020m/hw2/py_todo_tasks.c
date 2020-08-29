#include <Python.h>
#include <time.h>
#include "todo_api.h"
#include <stdio.h>
#include <sys/types.h>

static PyObject *
posix_error(void)
{
  return PyErr_SetFromErrno(PyExc_OSError);
}


static PyObject *
py_push_TODO(PyObject *self, PyObject *args)
{
  int pid;
  char *TODO;
  ssize_t TODO_size;
  int req_size;
  int status;
  
  if (!PyArg_ParseTuple(args, "iz#", &pid, &TODO, &req_size))
    return NULL;

  TODO_size = (ssize_t)req_size;
  
  status = push_TODO(pid, TODO, TODO_size);
  
  if (status < 0)
    return posix_error();
  
  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject *
py_peek_TODO(PyObject *self, PyObject *args)
{
  int pid;
  char TODO[40];
  int TODO_size;
  int status;

  if (!PyArg_ParseTuple(args, "ii", &pid, &TODO_size))
    return NULL;
 
  status = peek_TODO(pid, TODO, TODO_size);

  if (status < 0) {
    return posix_error();
  }

  return Py_BuildValue("si", TODO, (int)status);
}


static PyObject *
py_pop_TODO(PyObject *self, PyObject *args)
{
  int pid;
  int status;
  
  if (!PyArg_ParseTuple(args, "i", &pid))
    return NULL;
  status = pop_TODO(pid);
  if (status < 0) {
    return posix_error();
  }
  
  Py_INCREF(Py_None);
  return Py_None;
}


static PyMethodDef msgMethods[] = {
  {"push_TODO",  py_push_TODO, METH_VARARGS,
   "Add a TODO to a TODO's stack of a process identified by its PID.\nExample:\npush_TODO(1234, 'feed the dog') - will add the TODO task 'feed the dog' to the stack of process 1234.\nReturns - None."},
  {"peek_TODO",  py_peek_TODO, METH_VARARGS,
   "Get the description of the top-most TODO task, in the TODO's stack a of process identified by its pid with a maximum TODO description length.\nExample:\npeek_TODO(pid, TODO_size)\nReturns - A tuple whose first element is the description of the TODO task and the second is the TODO description size."},
  {"pop_TODO",  py_pop_TODO, METH_VARARGS,
   "Delete the top-most TODO in the TODO's stack of process pid.\nExample:\npop_TODO(pid)\nReturns - None."},
  {NULL, NULL, 0, NULL} 
};


void
initpyTodoTasks(void)
{
  (void) Py_InitModule("pyTodoTasks", msgMethods);
}
