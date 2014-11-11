#include "onlineldainterface.h"

#include <new>
using std::bad_alloc;

#include <vector>
using std::vector;

#include <utility>
using std::pair;

#include <iostream>
using std::cout;
using std::endl;

#include "trlda/utils"
using TRLDA::Exception;

#include "pyutils.h"

const char* OnlineLDA_doc =
	"An implementation of an online trust region method for latent dirichlet allocation (LDA).";

int OnlineLDA_init(OnlineLDAObject* self, PyObject* args, PyObject* kwds) {
	const char* kwlist[] = {
		"num_words",
		"num_topics",
		"num_documents",
		"alpha",
		"eta",
		"kappa_",
		"tau_", 0};

	int num_words;
	int num_topics;
	int num_documents;
	PyObject* alpha = 0;
	double eta = .3;

	// needed to support opening of old versions of pickled OnlineLDA objects
	double kappa_ = 0.;
	double tau_ = 0.;

	if(!PyArg_ParseTupleAndKeywords(args, kwds, "iii|Oddd", const_cast<char**>(kwlist),
			&num_words, &num_topics, &num_documents, &alpha, &eta, &kappa_, &tau_))
		return -1;

	try {
		if(alpha == 0) {
			self->lda = new OnlineLDA(num_words, num_topics, num_documents, .1, eta);
		} else if(PyFloat_Check(alpha)) {
			self->lda = new OnlineLDA(num_words, num_topics, num_documents, PyFloat_AsDouble(alpha), eta);
		} else if(PyInt_Check(alpha)) {
			self->lda = new OnlineLDA(num_words, num_topics, num_documents, PyInt_AsLong(alpha), eta);
		} else {
			alpha = PyArray_FROM_OTF(alpha, NPY_DOUBLE, NPY_IN_ARRAY);

			if(!alpha) {
				PyErr_SetString(PyExc_TypeError, "Alpha should be of type `ndarray`.");
				return -1;
			}

			MatrixXd alpha_ = PyArray_ToMatrixXd(alpha);

			if(alpha_.rows() == 1)
				alpha_ = alpha_.transpose();
			if(alpha_.cols() != 1) {
				PyErr_SetString(PyExc_TypeError, "Alpha should be one-dimensional.");
				return -1;
			}

			self->lda = new OnlineLDA(num_words, num_documents, alpha_, eta);
		}

	} catch(Exception& exception) {
		PyErr_SetString(PyExc_RuntimeError, exception.message());
	}

	return 0;
}



PyObject* OnlineLDA_num_topics(OnlineLDAObject* self, void*) {
	return PyInt_FromLong(self->lda->numTopics());
}



PyObject* OnlineLDA_num_words(OnlineLDAObject* self, void*) {
	return PyInt_FromLong(self->lda->numWords());
}



PyObject* OnlineLDA_num_documents(OnlineLDAObject* self, void*) {
	return PyInt_FromLong(self->lda->numDocuments());
}



int OnlineLDA_set_num_documents(OnlineLDAObject* self, PyObject* value, void*) {
	int num_documents = PyInt_AsLong(value);

	if(PyErr_Occurred())
		return -1;

	try {
		self->lda->setNumDocuments(num_documents);
	} catch(Exception exception) {
		PyErr_SetString(PyExc_RuntimeError, exception.message());
		return -1;
	}

	return 0;
}




PyObject* OnlineLDA_lambda(OnlineLDAObject* self, void*) {
	PyObject* array = PyArray_FromMatrixXd(self->lda->lambda());

	// make array immutable
	reinterpret_cast<PyArrayObject*>(array)->flags &= ~NPY_WRITEABLE;

	return array;
}



int OnlineLDA_set_lambda(OnlineLDAObject* self, PyObject* value, void*) {
	value = PyArray_FROM_OTF(value, NPY_DOUBLE, NPY_IN_ARRAY);

	if(!value) {
		PyErr_SetString(PyExc_TypeError, "Lambda should be of type `ndarray`.");
		return -1;
	}

	try {
		self->lda->setLambda(PyArray_ToMatrixXd(value));
	} catch(Exception exception) {
		Py_DECREF(value);
		PyErr_SetString(PyExc_RuntimeError, exception.message());
		return -1;
	}

	Py_DECREF(value);

	return 0;
}



PyObject* OnlineLDA_alpha(OnlineLDAObject* self, void*) {
	return PyArray_FromMatrixXd(self->lda->alpha());
}



int OnlineLDA_set_alpha(OnlineLDAObject* self, PyObject* alpha, void*) {
	try {
		if(PyFloat_Check(alpha)) {
			self->lda->setAlpha(PyFloat_AsDouble(alpha));
		} else if(PyInt_Check(alpha)) {
			self->lda->setAlpha(PyInt_AsLong(alpha));
		} else {
			alpha = PyArray_FROM_OTF(alpha, NPY_DOUBLE, NPY_IN_ARRAY);

			if(!alpha) {
				PyErr_SetString(PyExc_TypeError, "Alpha should be of type `ndarray`.");
				return -1;
			}

			MatrixXd alpha_ = PyArray_ToMatrixXd(alpha);

			if(alpha_.rows() == 1)
				alpha_ = alpha_.transpose();
			if(alpha_.cols() != 1) {
				PyErr_SetString(PyExc_TypeError, "Alpha should be one-dimensional.");
				return -1;
			}

			self->lda->setAlpha(alpha_);
		}
	} catch(Exception exception) {
		PyErr_SetString(PyExc_RuntimeError, exception.message());
		return -1;
	}

	return 0;
}



PyObject* OnlineLDA_eta(OnlineLDAObject* self, void*) {
	return PyFloat_FromDouble(self->lda->eta());
}



int OnlineLDA_set_eta(OnlineLDAObject* self, PyObject* value, void*) {
	double eta = PyFloat_AsDouble(value);

	if(PyErr_Occurred())
		return -1;

	try {
		self->lda->setEta(eta);
	} catch(Exception exception) {
		PyErr_SetString(PyExc_RuntimeError, exception.message());
		return -1;
	}

	return 0;
}



int PyList_ToDocuments(PyObject* docs, void* documents_) {
	OnlineLDA::Documents& documents = *reinterpret_cast<OnlineLDA::Documents*>(documents_);

	if(!PyList_Check(docs)) {
		PyErr_SetString(PyExc_TypeError, "Documents must be stored in a list.");
		return 0;
	}

	try {
		// create container for documents
		documents = OnlineLDA::Documents(PyList_Size(docs));

		// convert documents
		for(int i = 0; i < documents.size(); ++i) {
			PyObject* doc = PyList_GetItem(docs, i);

			// make sure document is a list
			if(!PyList_Check(doc)) {
				PyErr_SetString(PyExc_TypeError, "Each document must be a list of tuples.");
				return 0;
			}

			// create container for words
			documents[i] = OnlineLDA::Document(PyList_Size(doc));

			// load words
			for(int j = 0; j < documents[i].size(); ++j)
				if(!PyArg_ParseTuple(PyList_GetItem(doc, j), "ii",
					&documents[i][j].first,
					&documents[i][j].second))
					return 0;
		}
	} catch(bad_alloc&) {
		PyErr_SetString(PyExc_TypeError, "Not enough memory.");
		return 0;
	}

	return 1;
}



PyObject* PyList_FromDocuments(const OnlineLDA::Documents& documents) {
	PyObject* documents_ = PyList_New(0);

	for(int n = 0; n < documents.size(); ++n) {
		PyObject* document = PyList_New(0);

		for(int i = 0; i < documents[n].size(); ++i) {
			const int& wordID = documents[n][i].first;
			const int& wordCount = documents[n][i].second;

			PyObject* tuple = Py_BuildValue("(ii)", wordID, wordCount);
			PyList_Append(document, tuple);
			Py_DECREF(tuple);
		}

		PyList_Append(documents_, document);
		Py_DECREF(document);
	}

	return documents_;
}



const char* OnlineLDA_sample_doc =
	"";

PyObject* OnlineLDA_sample(
	OnlineLDAObject* self,
	PyObject* args,
	PyObject* kwds)
{
	const char* kwlist[] = {"num_documents", "length", 0};

	int num_documents;
	int length;

	// parse arguments
	if(!PyArg_ParseTupleAndKeywords(args, kwds, "ii", const_cast<char**>(kwlist),
			&num_documents, &length))
		return 0;

	try {
		// return list of documents
		return PyList_FromDocuments(self->lda->sample(num_documents, length));
	} catch(Exception& exception) {
		PyErr_SetString(PyExc_RuntimeError, exception.message());
		return 0;
	}

	return 0;
}



const char* OnlineLDA_update_variables_doc =
	"";

PyObject* OnlineLDA_update_variables(
	OnlineLDAObject* self,
	PyObject* args,
	PyObject* kwds)
{
	const char* kwlist[] = {"docs", "latents", "inference_method", "max_iter", "num_samples", "burn_in", 0};

	OnlineLDA::Documents documents;
	OnlineLDA::Parameters parameters;
	PyObject* latents = 0;
	const char* inference_method = 0;

	// parse arguments
	if(!PyArg_ParseTupleAndKeywords(args, kwds, "O&|Osiii", const_cast<char**>(kwlist),
			&PyList_ToDocuments, &documents,
			&latents,
			&inference_method,
			&parameters.maxIterInference,
			&parameters.numSamples,
			&parameters.burnIn))
		return 0;

	if(latents) {
		// make sure latents is a NumPy array
		latents = PyArray_FROM_OTF(latents, NPY_DOUBLE, NPY_IN_ARRAY);
		if(!latents) {
			PyErr_SetString(PyExc_TypeError, "`latents` should be of type `ndarray`.");
			return 0;
		}
	}

	if(inference_method) {
		switch(inference_method[0]) {
			case 'g':
			case 'G':
				parameters.inferenceMethod = OnlineLDA::GIBBS;
				break;

			case 'v':
			case 'V':
				parameters.inferenceMethod = OnlineLDA::VI;
				break;

			default:
				PyErr_SetString(PyExc_TypeError, "`inference_method` should be either 'gibbs' or 'vi'.");
				return 0;
		}
	}

	try {
		pair<ArrayXXd, ArrayXXd> results;

		if(latents)
			results = self->lda->updateVariables(
				documents,
				PyArray_ToMatrixXd(latents),
				parameters);
		else
			results = self->lda->updateVariables(documents, parameters);

		PyObject* rlatents = PyArray_FromMatrixXd(results.first);
		PyObject* sstats = PyArray_FromMatrixXd(results.second);
		PyObject* result = Py_BuildValue("(OO)", rlatents, sstats);

		Py_DECREF(rlatents);
		Py_DECREF(sstats);

		return result;

	} catch(Exception& exception) {
		PyErr_SetString(PyExc_RuntimeError, exception.message());
		Py_XDECREF(latents);
		return 0;
	}

	Py_XDECREF(latents);

	return 0;
}



const char* OnlineLDA_update_parameters_doc =
	"";

PyObject* OnlineLDA_update_parameters(
	OnlineLDAObject* self,
	PyObject* args,
	PyObject* kwds)
{
	const char* kwlist[] = {
		"docs",
		"max_iter_tr",
		"max_iter_inference",
		"kappa",
		"tau",
		"rho",
		"adaptive",
		"init_gamma",
		"update_lambda",
		"update_alpha",
		"update_eta",
		"min_alpha",
		"min_eta", 0};

	OnlineLDA::Documents documents;
	OnlineLDA::Parameters parameters;

	// parse arguments
	if(!PyArg_ParseTupleAndKeywords(args, kwds, "O&|iidddbbbbbdd", const_cast<char**>(kwlist),
			&PyList_ToDocuments, &documents,
			&parameters.maxIterTR,
			&parameters.maxIterInference,
			&parameters.kappa,
			&parameters.tau,
			&parameters.rho,
			&parameters.adaptive,
			&parameters.initGamma,
			&parameters.updateLambda,
			&parameters.updateAlpha,
			&parameters.updateEta,
			&parameters.minAlpha,
			&parameters.minEta))
		return 0;

	try {
		// return learning rate used
		return PyFloat_FromDouble(self->lda->updateParameters(documents, parameters));
	} catch(Exception& exception) {
		PyErr_SetString(PyExc_RuntimeError, exception.message());
		return 0;
	}

	return 0;
}



const char* OnlineLDA_reduce_doc =
	"__reduce__(self)\n"
	"\n"
	"Method used by Pickle.";

PyObject* OnlineLDA_reduce(OnlineLDAObject* self, PyObject*) {
	PyObject* alpha = PyArray_FromMatrixXd(self->lda->alpha());

	// constructor arguments
	PyObject* args = Py_BuildValue("(iiiOd)",
		self->lda->numWords(),
		self->lda->numTopics(),
		self->lda->numDocuments(),
		alpha,
		self->lda->eta());

	Py_DECREF(alpha);

	PyObject* lambda = OnlineLDA_lambda(self, 0);
	PyObject* state = Py_BuildValue("(Oi)", lambda, self->lda->updateCount());
	PyObject* result = Py_BuildValue("(OOO)", Py_TYPE(self), args, state);

	Py_DECREF(lambda);
	Py_DECREF(args);
	Py_DECREF(state);

	return result;
}



const char* OnlineLDA_setstate_doc =
	"__setstate__(self)\n"
	"\n"
	"Method used by Pickle.";

PyObject* OnlineLDA_setstate(OnlineLDAObject* self, PyObject* state) {
	PyObject* lambda;
	int updateCount;

	if(!PyArg_ParseTuple(state, "(Oi)", &lambda, &updateCount))
		return 0;

	try {
		OnlineLDA_set_lambda(self, lambda, 0);
		self->lda->setUpdateCount(updateCount);
	} catch(Exception exception) {
		PyErr_SetString(PyExc_RuntimeError, exception.message());
		return 0;
	}

	Py_INCREF(Py_None);
	return Py_None;
}
