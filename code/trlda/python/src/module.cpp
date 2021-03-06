#define PY_ARRAY_UNIQUE_SYMBOL TRLDA_ARRAY_API
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include <arrayobject.h>
#include <stdlib.h>
#include <sys/time.h>
#include "distributioninterface.h"
#include "ldainterface.h"
#include "onlineldainterface.h"
#include "batchldainterface.h"
#include "cumulativeldainterface.h"
#include "utilsinterface.h"
#include "Eigen/Core"

static PyGetSetDef Distribution_getset[] = {
	{0}
};

static PyMethodDef Distribution_methods[] = {
	{0}
};

PyTypeObject Distribution_type = {
	PyObject_HEAD_INIT(0)
	0,                                    /*ob_size*/
	"trlda.models.Distribution",          /*tp_name*/
	sizeof(DistributionObject),           /*tp_basicsize*/
	0,                                    /*tp_itemsize*/
	(destructor)Distribution_dealloc,     /*tp_dealloc*/
	0,                                    /*tp_print*/
	0,                                    /*tp_getattr*/
	0,                                    /*tp_setattr*/
	0,                                    /*tp_compare*/
	0,                                    /*tp_repr*/
	0,                                    /*tp_as_number*/
	0,                                    /*tp_as_sequence*/
	0,                                    /*tp_as_mapping*/
	0,                                    /*tp_hash */
	0,                                    /*tp_call*/
	0,                                    /*tp_str*/
	0,                                    /*tp_getattro*/
	0,                                    /*tp_setattro*/
	0,                                    /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,                   /*tp_flags*/
	Distribution_doc,                     /*tp_doc*/
	0,                                    /*tp_traverse*/
	0,                                    /*tp_clear*/
	0,                                    /*tp_richcompare*/
	0,                                    /*tp_weaklistoffset*/
	0,                                    /*tp_iter*/
	0,                                    /*tp_iternext*/
	Distribution_methods,                 /*tp_methods*/
	0,                                    /*tp_members*/
	Distribution_getset,                  /*tp_getset*/
	0,                                    /*tp_base*/
	0,                                    /*tp_dict*/
	0,                                    /*tp_descr_get*/
	0,                                    /*tp_descr_set*/
	0,                                    /*tp_dictoffset*/
	(initproc)Distribution_init,          /*tp_init*/
	0,                                    /*tp_alloc*/
	Distribution_new,                     /*tp_new*/
};

static PyGetSetDef LDA_getset[] = {
	{"num_topics",
		(getter)LDA_num_topics,
		0,
		"Number of topics."},
	{"num_words",
		(getter)LDA_num_words,
		0,
		"Number of words."},
	{"lambdas",
		(getter)LDA_lambda,
		(setter)LDA_set_lambda,
		"Parameters governing beliefs over topics, $\\beta_{ki}$."},
	{"_lambda",
		(getter)LDA_lambda,
		(setter)LDA_set_lambda,
		"Alias for L{lambdas}."},
	{"alpha",
		(getter)LDA_alpha,
		(setter)LDA_set_alpha,
		"Controls Dirichlet prior over topic weights, $\\theta_k$."},
	{"eta",
		(getter)LDA_eta,
		(setter)LDA_set_eta,
		"Controls Dirichlet prior over topics, $\\beta_{ki}$."},
	{0}
};

static PyMethodDef LDA_methods[] = {
	{"sample",
		(PyCFunction)LDA_sample,
		METH_VARARGS | METH_KEYWORDS,
		LDA_sample_doc},
	{"update_variables",
		(PyCFunction)LDA_update_variables,
		METH_VARARGS | METH_KEYWORDS,
		LDA_update_variables_doc},
	{"do_e_step",
		(PyCFunction)LDA_update_variables,
		METH_VARARGS | METH_KEYWORDS,
		"Alias for L{update_variables}."},
    {"lower_bound",
		(PyCFunction)LDA_lower_bound,
		METH_VARARGS | METH_KEYWORDS,
		LDA_lower_bound_doc},
	{0}
};

PyTypeObject LDA_type = {
	PyObject_HEAD_INIT(0)
	0,                                /*ob_size*/
	"trlda.models.LDA",               /*tp_name*/
	sizeof(LDAObject),                /*tp_basicsize*/
	0,                                /*tp_itemsize*/
	(destructor)Distribution_dealloc, /*tp_dealloc*/
	0,                                /*tp_print*/
	0,                                /*tp_getattr*/
	0,                                /*tp_setattr*/
	0,                                /*tp_compare*/
	0,                                /*tp_repr*/
	0,                                /*tp_as_number*/
	0,                                /*tp_as_sequence*/
	0,                                /*tp_as_mapping*/
	0,                                /*tp_hash */
	0,                                /*tp_call*/
	LDA_str,                          /*tp_str*/
	0,                                /*tp_getattro*/
	0,                                /*tp_setattro*/
	0,                                /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,               /*tp_flags*/
	LDA_doc,                          /*tp_doc*/
	0,                                /*tp_traverse*/
	0,                                /*tp_clear*/
	0,                                /*tp_richcompare*/
	0,                                /*tp_weaklistoffset*/
	0,                                /*tp_iter*/
	0,                                /*tp_iternext*/
	LDA_methods,                      /*tp_methods*/
	0,                                /*tp_members*/
	LDA_getset,                       /*tp_getset*/
	&Distribution_type,               /*tp_base*/
	0,                                /*tp_dict*/
	0,                                /*tp_descr_get*/
	0,                                /*tp_descr_set*/
	0,                                /*tp_dictoffset*/
	(initproc)LDA_init,               /*tp_init*/
	0,                                /*tp_alloc*/
	Distribution_new,                 /*tp_new*/
};

static PyGetSetDef OnlineLDA_getset[] = {
	{"num_documents",
		(getter)OnlineLDA_num_documents,
		(setter)OnlineLDA_set_num_documents,
		"Number of documents in the complete dataset."},
	{"update_count",
		(getter)OnlineLDA_update_count,
		(setter)OnlineLDA_set_update_count,
		"Number of parameter updates."},
	{0}
};

static PyMethodDef OnlineLDA_methods[] = {
	{"update_parameters",
		(PyCFunction)OnlineLDA_update_parameters,
		METH_VARARGS | METH_KEYWORDS,
		OnlineLDA_update_parameters_doc},
	{"__reduce__", (PyCFunction)OnlineLDA_reduce, METH_NOARGS, 0},
	{"__setstate__", (PyCFunction)OnlineLDA_setstate, METH_VARARGS, 0},
	{0}
};

PyTypeObject OnlineLDA_type = {
	PyObject_HEAD_INIT(0)
	0,                                /*ob_size*/
	"trlda.models.OnlineLDA",         /*tp_name*/
	sizeof(OnlineLDAObject),          /*tp_basicsize*/
	0,                                /*tp_itemsize*/
	(destructor)Distribution_dealloc, /*tp_dealloc*/
	0,                                /*tp_print*/
	0,                                /*tp_getattr*/
	0,                                /*tp_setattr*/
	0,                                /*tp_compare*/
	0,                                /*tp_repr*/
	0,                                /*tp_as_number*/
	0,                                /*tp_as_sequence*/
	0,                                /*tp_as_mapping*/
	0,                                /*tp_hash */
	0,                                /*tp_call*/
	0,                                /*tp_str*/
	0,                                /*tp_getattro*/
	0,                                /*tp_setattro*/
	0,                                /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,               /*tp_flags*/
	OnlineLDA_doc,                    /*tp_doc*/
	0,                                /*tp_traverse*/
	0,                                /*tp_clear*/
	0,                                /*tp_richcompare*/
	0,                                /*tp_weaklistoffset*/
	0,                                /*tp_iter*/
	0,                                /*tp_iternext*/
	OnlineLDA_methods,                /*tp_methods*/
	0,                                /*tp_members*/
	OnlineLDA_getset,                 /*tp_getset*/
	&LDA_type,                        /*tp_base*/
	0,                                /*tp_dict*/
	0,                                /*tp_descr_get*/
	0,                                /*tp_descr_set*/
	0,                                /*tp_dictoffset*/
	(initproc)OnlineLDA_init,         /*tp_init*/
	0,                                /*tp_alloc*/
	Distribution_new,                 /*tp_new*/
};

static PyGetSetDef BatchLDA_getset[] = {
	{0}
};

static PyMethodDef BatchLDA_methods[] = {
	{"update_parameters",
		(PyCFunction)BatchLDA_update_parameters,
		METH_VARARGS | METH_KEYWORDS,
		BatchLDA_update_parameters_doc},
	{"__reduce__", (PyCFunction)BatchLDA_reduce, METH_NOARGS, 0},
	{"__setstate__", (PyCFunction)BatchLDA_setstate, METH_VARARGS, 0},
	{0}
};

PyTypeObject BatchLDA_type = {
	PyObject_HEAD_INIT(0)
	0,                                /*ob_size*/
	"trlda.models.BatchLDA",          /*tp_name*/
	sizeof(BatchLDAObject),           /*tp_basicsize*/
	0,                                /*tp_itemsize*/
	(destructor)Distribution_dealloc, /*tp_dealloc*/
	0,                                /*tp_print*/
	0,                                /*tp_getattr*/
	0,                                /*tp_setattr*/
	0,                                /*tp_compare*/
	0,                                /*tp_repr*/
	0,                                /*tp_as_number*/
	0,                                /*tp_as_sequence*/
	0,                                /*tp_as_mapping*/
	0,                                /*tp_hash */
	0,                                /*tp_call*/
	0,                                /*tp_str*/
	0,                                /*tp_getattro*/
	0,                                /*tp_setattro*/
	0,                                /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,               /*tp_flags*/
	BatchLDA_doc,                     /*tp_doc*/
	0,                                /*tp_traverse*/
	0,                                /*tp_clear*/
	0,                                /*tp_richcompare*/
	0,                                /*tp_weaklistoffset*/
	0,                                /*tp_iter*/
	0,                                /*tp_iternext*/
	BatchLDA_methods,                 /*tp_methods*/
	0,                                /*tp_members*/
	BatchLDA_getset,                  /*tp_getset*/
	&LDA_type,                        /*tp_base*/
	0,                                /*tp_dict*/
	0,                                /*tp_descr_get*/
	0,                                /*tp_descr_set*/
	0,                                /*tp_dictoffset*/
	(initproc)BatchLDA_init,          /*tp_init*/
	0,                                /*tp_alloc*/
	Distribution_new,                 /*tp_new*/
};

static PyGetSetDef CumulativeLDA_getset[] = {
	{0}
};

static PyMethodDef CumulativeLDA_methods[] = {
	{"update_parameters",
		(PyCFunction)CumulativeLDA_update_parameters,
		METH_VARARGS | METH_KEYWORDS,
		CumulativeLDA_update_parameters_doc},
	{"__reduce__", (PyCFunction)CumulativeLDA_reduce, METH_NOARGS, 0},
	{"__setstate__", (PyCFunction)CumulativeLDA_setstate, METH_VARARGS, 0},
	{0}
};

PyTypeObject CumulativeLDA_type = {
	PyObject_HEAD_INIT(0)
	0,                                /*ob_size*/
	"trlda.models.CumulativeLDA",     /*tp_name*/
	sizeof(CumulativeLDAObject),      /*tp_basicsize*/
	0,                                /*tp_itemsize*/
	(destructor)Distribution_dealloc, /*tp_dealloc*/
	0,                                /*tp_print*/
	0,                                /*tp_getattr*/
	0,                                /*tp_setattr*/
	0,                                /*tp_compare*/
	0,                                /*tp_repr*/
	0,                                /*tp_as_number*/
	0,                                /*tp_as_sequence*/
	0,                                /*tp_as_mapping*/
	0,                                /*tp_hash */
	0,                                /*tp_call*/
	0,                                /*tp_str*/
	0,                                /*tp_getattro*/
	0,                                /*tp_setattro*/
	0,                                /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,               /*tp_flags*/
	CumulativeLDA_doc,                /*tp_doc*/
	0,                                /*tp_traverse*/
	0,                                /*tp_clear*/
	0,                                /*tp_richcompare*/
	0,                                /*tp_weaklistoffset*/
	0,                                /*tp_iter*/
	0,                                /*tp_iternext*/
	CumulativeLDA_methods,            /*tp_methods*/
	0,                                /*tp_members*/
	CumulativeLDA_getset,             /*tp_getset*/
	&LDA_type,                        /*tp_base*/
	0,                                /*tp_dict*/
	0,                                /*tp_descr_get*/
	0,                                /*tp_descr_set*/
	0,                                /*tp_dictoffset*/
	(initproc)CumulativeLDA_init,     /*tp_init*/
	0,                                /*tp_alloc*/
	Distribution_new,                 /*tp_new*/
};

PyObject* seed(PyObject* self, PyObject* args, PyObject* kwds) {
	int seed;

	if(!PyArg_ParseTuple(args, "i", &seed))
		return 0;

	srand(seed);

	Py_INCREF(Py_None);
	return Py_None;
}

static const char* trlda_doc =
	"An implementation of an online trust region method for latent dirichlet allocation (LDA).";

static PyMethodDef trlda_methods[] = {
	{"seed", (PyCFunction)seed, METH_VARARGS, 0},
	{"random_select", (PyCFunction)random_select, METH_VARARGS | METH_KEYWORDS, random_select_doc},
	{"sample_dirichlet", (PyCFunction)sample_dirichlet, METH_VARARGS | METH_KEYWORDS, sample_dirichlet_doc},
	{"polygamma", (PyCFunction)polygamma, METH_VARARGS | METH_KEYWORDS, 0},
	{0}
};

PyMODINIT_FUNC init_trlda() {
	// set random seed
	timeval time;
	gettimeofday(&time, 0);
	srand(time.tv_usec * time.tv_sec);

	// initialize NumPy
	import_array();

	// initialize Eigen
	Eigen::initParallel();

	// create module object
	PyObject* module = Py_InitModule3("_trlda", trlda_methods, trlda_doc);

	// initialize types
	if(PyType_Ready(&LDA_type) < 0)
		return;
	if(PyType_Ready(&OnlineLDA_type) < 0)
		return;
	if(PyType_Ready(&BatchLDA_type) < 0)
		return;
	if(PyType_Ready(&CumulativeLDA_type) < 0)
		return;

	Py_INCREF(&Distribution_type);
	Py_INCREF(&LDA_type);
	Py_INCREF(&BatchLDA_type);
	Py_INCREF(&CumulativeLDA_type);

	// add types to module
	PyModule_AddObject(module, "Distribution", reinterpret_cast<PyObject*>(&Distribution_type));
	PyModule_AddObject(module, "LDA", reinterpret_cast<PyObject*>(&LDA_type));
	PyModule_AddObject(module, "OnlineLDA", reinterpret_cast<PyObject*>(&OnlineLDA_type));
	PyModule_AddObject(module, "BatchLDA", reinterpret_cast<PyObject*>(&BatchLDA_type));
	PyModule_AddObject(module, "CumulativeLDA", reinterpret_cast<PyObject*>(&CumulativeLDA_type));
}
