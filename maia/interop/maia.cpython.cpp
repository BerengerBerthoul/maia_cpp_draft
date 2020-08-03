#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "cpp_cgns/interop/pycgns_converter.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "maia/transform/partition_with_boundary_first/partition_with_boundary_first.hpp"
#include "maia/transform/convert_to_simple_connectivities.hpp"
#include "maia/transform/remove_ghost_info.hpp"
#include "maia/generate/nfaces_from_ngons.hpp"

static PyObject*
partition_with_boundary_first(PyObject* self, PyObject* args) {
  PyObject* base_pytree;
  if (!PyArg_ParseTuple(args, "O", &base_pytree)) return NULL;
  cgns::tree base = cgns::view_as_cpptree(base_pytree);

  cgns::cgns_allocator alloc; // allocates and owns memory
  cgns::partition_with_boundary_first(base,cgns::factory(&alloc));

  add_new_nodes_and_ownership(base,alloc,base_pytree);
  Py_INCREF(Py_None); return Py_None;
}

static PyObject*
sort_nface_into_simple_connectivities(PyObject* self, PyObject* args) {
  PyObject* base_pytree;
  if (!PyArg_ParseTuple(args, "O", &base_pytree)) return NULL;
  cgns::tree base = cgns::view_as_cpptree(base_pytree);

  cgns::cgns_allocator alloc; // allocates and owns memory
  cgns::sort_nface_into_simple_connectivities(base,cgns::factory(&alloc));

  add_new_nodes_and_ownership(base,alloc,base_pytree);
  Py_INCREF(Py_None); return Py_None;
}

static PyObject*
convert_to_simple_connectivities(PyObject* self, PyObject* args) {
  PyObject* base_pytree;
  if (!PyArg_ParseTuple(args, "O", &base_pytree)) return NULL;
  cgns::tree base = cgns::view_as_cpptree(base_pytree);

  cgns::cgns_allocator alloc; // allocates and owns memory
  cgns::convert_to_simple_connectivities(base,cgns::factory(&alloc));

  //add_new_nodes_and_ownership(base,alloc,base_pytree);
  update_and_transfer_ownership(base,alloc,base_pytree);
  Py_INCREF(Py_None); return Py_None;
}

static PyObject*
add_nfaces(PyObject* self, PyObject* args) {
  PyObject* base_pytree;
  if (!PyArg_ParseTuple(args, "O", &base_pytree)) return NULL;
  cgns::tree base = cgns::view_as_cpptree(base_pytree);

  cgns::cgns_allocator alloc; // allocates and owns memory
  cgns::add_nfaces(base,cgns::factory(&alloc));

  add_new_nodes_and_ownership(base,alloc,base_pytree);
  Py_INCREF(Py_None); return Py_None;
}

static PyObject*
remove_ghost_info(PyObject* self, PyObject* args) {
  PyObject* base_pytree;
  if (!PyArg_ParseTuple(args, "O", &base_pytree)) return NULL;
  cgns::tree base = cgns::view_as_cpptree(base_pytree);

  cgns::cgns_allocator alloc; // allocates and owns memory
  cgns::remove_ghost_info(base,cgns::factory(&alloc));

  update_and_transfer_ownership(base,alloc,base_pytree);
  Py_INCREF(Py_None); return Py_None;
}

static PyMethodDef maia_methods[] = {
  {"partition_with_boundary_first"        , partition_with_boundary_first        , METH_VARARGS, "ngon sorted with boundary faces first"},
  {"sort_nface_into_simple_connectivities", sort_nface_into_simple_connectivities, METH_VARARGS, "sort nface into tet, prism, pyra, hex"},
  {"convert_to_simple_connectivities"     , convert_to_simple_connectivities     , METH_VARARGS, "turn ngon with boundary first and nface to tri, quad, tet, prism, pyra, hex"},
  {"add_nfaces"                           , add_nfaces                           , METH_VARARGS, "add nface Elements_t from ngons with ParentElements"},
  {"remove_ghost_info"                    , remove_ghost_info                    , METH_VARARGS, "Remove ghost nodes and ghost elements of base"},
  {NULL, NULL, 0, NULL} /* Sentinel */
};

static PyModuleDef maia_module = {
  PyModuleDef_HEAD_INIT,
  "maia_cpython", /* name of module */
  NULL, /* module documentation, may be NULL */
  -1, /* size of per-interpreter state of the module,
         or -1 if the module keeps state in global variables */
  maia_methods
};

PyMODINIT_FUNC
PyInit_maia_cpython(void) {
  return PyModule_Create(&maia_module);
}
