#include <Python.h>
#include <time.h>
#include "policy_api.h"
#include <stdio.h>
#include <sys/types.h>

static PyObject *
posix_error(void)
{
  return PyErr_SetFromErrno(PyExc_OSError);
}


static PyObject *
py_set_policy(PyObject *self, PyObject *args)
{
  int pid;
  int policy_id;
  int policy_value;
  int status;
  
  if (!PyArg_ParseTuple(args, "iii", &pid, &policy_id, &policy_value))
    return NULL;

  status = set_policy(pid, policy_id, policy_value);
  
  if (status < 0)
    return posix_error();
  
  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject *
py_get_policy(PyObject *self, PyObject *args)
{
  int pid;
  int policy_id;
  int policy_value;
  int status;

  if (!PyArg_ParseTuple(args, "i", &pid))
    return NULL;
 
  status = get_policy(pid, &policy_id, &policy_value);

  if (status < 0) {
    return posix_error();
  }

  return Py_BuildValue("ii", policy_id, policy_value);
}

static PyMethodDef msgMethods[] = {
  {"set_policy",  py_set_policy, METH_VARARGS,
   "Sets a policy for a process identified by its PID.\nExample:\nset_policy(1234, 1, 10) - will set the policy with policy_id 1 and policy value of 10 to the process 1234.\nReturns - None."},
  {"get_policy",  py_get_policy, METH_VARARGS,
   "Gets the policy associated with the process identified by pid.\nExample:\nget_policy(pid)\nReturns - A tuple whose first element is the policy_id and the second element is the policy_value."},
  {NULL, NULL, 0, NULL} 
};


void
initpyPolicy(void)
{
  (void) Py_InitModule("pyPolicy", msgMethods);
}
