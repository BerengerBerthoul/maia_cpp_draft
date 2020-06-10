#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "cpp_cgns/interop/pycgns_converter.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "maia/transform/partition_with_boundary_first/partition_with_boundary_first.hpp"

//static PyObject*
//maia_partition_with_boundary_first(PyObject* self, PyObject* args) {
//  PyObject* base_pytree;
//  if (!PyArg_ParseTuple(args, "O", &base_pytree)) return NULL;
//  
//  cgns::tree base = cgns::view_as_cpptree(base_pytree);
//
//  cgns::cgns_allocator alloc; // allocates and owns memory
//  //cgns::partition_with_boundary_first(base,cgns::factory(&alloc));
//
//  //pytree_with_transfered_ownership_inplace(base,alloc,base_pytree);
//
//  //Py_INCREF(Py_None); return Py_None;
// 
//  return pytree_with_transfered_ownership(base,alloc);
//}
static PyObject*
maia_partition_with_boundary_first(PyObject* self, PyObject* args) {
  PyObject* base_pytree;
  if (!PyArg_ParseTuple(args, "O", &base_pytree)) return NULL;
  
  cgns::tree base = cgns::view_as_cpptree(base_pytree);

  cgns::cgns_allocator alloc; // allocates and owns memory
  //cgns::partition_with_boundary_first(base,cgns::factory(&alloc));

  //pytree_with_transfered_ownership_inplace(base,alloc,base_pytree);

  //Py_INCREF(Py_None); return Py_None;
 
  return pytree_with_transfered_ownership(base,alloc);
}

static PyMethodDef maia_methods[] = {
  {"partition_with_boundary_first", maia_partition_with_boundary_first, METH_VARARGS, "ngon sorted with boundary faces first"},
  {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyModuleDef maiamodule = {
  PyModuleDef_HEAD_INIT,
  "maia", /* name of module */
  NULL, /* module documentation, may be NULL */
  -1, /* size of per-interpreter state of the module,
         or -1 if the module keeps state in global variables */
  maia_methods
};

PyMODINIT_FUNC
PyInit_maia(void) {
  return PyModule_Create(&maiamodule);
}
