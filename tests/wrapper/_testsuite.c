/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <stdint.h>

#include <Python.h>

#include "protocol/protocol.h"

typedef struct {
	/* clang-format off */
	PyObject_HEAD
	struct protocol_config_t config;
	/* clang-format on */
} DeviceObject;

/* firmware callbacks */

int hal_hid_send(struct hid_hal_t interface, u8 *buffer, size_t buffer_size)
{
	int rc = 0;
	DeviceObject *self = interface.drv_data;
	PyObject *callback, *bytes, *list;
	PyGILState_STATE state;

	callback = PyObject_GetAttrString((PyObject *) self, "hid_send");
	if (!callback) {
		PyErr_Clear();
		return rc;
	}

	if (!PyCallable_Check(callback)) {
		PyErr_SetString(PyExc_TypeError, "hid_send should be callable");
		rc = ENOEXEC;
		goto error;
	}

	bytes = PyBytes_FromStringAndSize((char *) buffer, buffer_size);
	if (!bytes)
		goto error;

	list = PySequence_List(bytes);
	if (!list)
		goto error_bytes;

	state = PyGILState_Ensure();
	PyObject_CallOneArg(callback, list);
	PyGILState_Release(state);

error_bytes:
	Py_DECREF(bytes);
error:
	Py_DECREF(callback);
	return rc;
}

/* Device class methods */

static PyObject *Device_protocol_dispatch(DeviceObject *self, PyObject *args)
{
	PyObject *data = NULL, *bytes = NULL;

	if (!PyArg_ParseTuple(args, "O", &data))
		goto error;

	bytes = PyBytes_FromObject(data);
	if (!bytes)
		goto error;

	protocol_dispatch(self->config, (u8 *) PyBytes_AsString(bytes), PyBytes_Size(bytes));

	Py_DECREF(bytes);

	Py_XINCREF(Py_None);
	return Py_None;

error:
	return NULL;
}

/* Device constructor and destructor */

static int Device_init(DeviceObject *self, PyObject *args, PyObject *kw)
{
	int rc = -1;
	static char *keywords[] = {"name", "functions", NULL};
	PyObject *functions = NULL, *page = NULL, *page_bytes = NULL;
	Py_ssize_t page_count, page_index, function_count;

	if (!PyArg_ParseTupleAndKeywords(args, kw, "sO", keywords, &self->config.device_name, &functions))
		goto error;

	/* construct functions array from python list */

	if (!PyList_Check(functions)) {
		PyErr_Format(PyExc_TypeError, "Invalid functions value (expecting list, got %s)", Py_TYPE(functions)->tp_name);
		goto error;
	}

	page_count = PyList_Size(functions);
	if (page_count != PAGE_COUNT) {
		PyErr_Format(PyExc_ValueError, "Invalid function list size (expecting %d, got %d)", PAGE_COUNT, page_count);
		goto error;
	}

	for (page_index = 0; page_index < page_count; page_index++) {
		/* get page, convert it to bytes, and fill the functions array */

		page = PyList_GetItem(functions, page_index);
		if (!page)
			goto error;

		page_bytes = PyBytes_FromObject(page);
		if (!page_bytes)
			goto error;

		/* alocate function page array */
		function_count = PyBytes_Size(page_bytes);
		self->config.functions[page_index] = PyMem_Calloc(sizeof(u8), function_count);
		self->config.functions_size[page_index] = function_count;

		memcpy(self->config.functions[page_index], PyBytes_AsString(page_bytes), sizeof(u8) * function_count);

		Py_DECREF(page_bytes);
		Py_DECREF(page);
	}

	self->config.hid_hal = (struct hid_hal_t){
		.send = hal_hid_send,
		.drv_data = self,
	};

	rc = 0;

error:
	return rc;
}

static void Device_dealloc(DeviceObject *self)
{
	for (size_t i = 0; i < PAGE_COUNT; i++) PyMem_Free(self->config.functions[i]);
	Py_TYPE(self)->tp_free((PyObject *) self);
}

/* Device class definition */

static PyMethodDef Device_methods[] = {
	{"protocol_dispatch", (PyCFunction) Device_protocol_dispatch, METH_VARARGS | METH_KEYWORDS, NULL},
	{NULL, NULL, 0, NULL}};

static PyTypeObject DeviceType = {
	/* clang-format off */
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "_testsuite.Device",
	.tp_doc = "Test device",
	.tp_basicsize = sizeof(DeviceObject),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_new = PyType_GenericNew,
	.tp_init = (initproc) Device_init,
	.tp_dealloc = (destructor) Device_dealloc,
	.tp_methods = Device_methods,
	/* clang-format on */
};

/* module definition */

static struct PyModuleDef testsuite_module = {
	/* clang-format off */
	PyModuleDef_HEAD_INIT,
	"_testsuite",
	"Wrapper module for the openinput testsuite target.",
	-1,
	NULL, //testsuite_methods,
	/* clang-format on */
};

PyMODINIT_FUNC PyInit__testsuite(void)
{
	Py_Initialize();

	PyObject *m = NULL;

	if (PyType_Ready(&DeviceType) < 0)
		return NULL;

	if ((m = PyModule_Create(&testsuite_module)) == NULL)
		return NULL;

	Py_XINCREF(&DeviceType);

	PyModule_AddObject(m, "Device", (PyObject *) &DeviceType);

	return m;
}
