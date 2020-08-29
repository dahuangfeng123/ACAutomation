#include <Python.h>
#include "_ACAutomation.h"
/****** wrap for python ********/

// for python3 use PyCapsule_GetPointer instead of PyCObject_AsVoidPtr
// ref: http://www.speedupcode.com/c-class-in-python3/

#if PY_MAJOR_VERSION >= 3
    #define PyCObject_FromVoidPtr(pointer, destructor) \
     (PyCapsule_New(pointer, "ACAutomation", destructor))
    #define PyCObject_AsVoidPtr(capsule) \
     (PyCapsule_GetPointer(capsule, "ACAutomation"))
#endif


#if PY_MAJOR_VERSION < 3
    static void ACAutomation_Delete(void *ptr)
    {
        ACAutomation * pAC = static_cast<ACAutomation *>(ptr);
        delete pAC;
    }
#else
    static void ACAutomation_Delete(PyObject *ptr)
    {
        ACAutomation * pAC = static_cast<ACAutomation *>(PyCapsule_GetPointer(ptr, "ACAutomation"));
        delete pAC;
    }
#endif 


static PyObject* ACAutomation_new(PyObject *self, PyObject* args)
{
    ACAutomation *pAC = new ACAutomation();
    return PyCObject_FromVoidPtr(pAC, ACAutomation_Delete);
}

static PyObject* ACAutomation_insert(PyObject *self, PyObject* args)
{
    PyObject *ac = 0;
    const char* str;
    int ok = PyArg_ParseTuple(args, "Os", &ac, &str);
    if(!ok) return NULL;
    //printf("insert: %s\n", str);
    ACAutomation *acautomation = static_cast<ACAutomation *>(PyCObject_AsVoidPtr(ac));
    acautomation->insert(str);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* ACAutomation_build(PyObject *self, PyObject* args)
{
    PyObject *ac = 0;
    int ok = PyArg_ParseTuple(args, "O", &ac);
    if(!ok) return NULL;
    ACAutomation *acautomation = static_cast<ACAutomation *>(PyCObject_AsVoidPtr(ac));
    acautomation->build();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* ACAutomation_matchOne(PyObject *self, PyObject* args)
{
    PyObject *ac = 0;
    const char* str;
    std::queue<Result> results;
    int ok = PyArg_ParseTuple(args, "Os", &ac, &str);
    if(!ok) return NULL;
    ACAutomation *acautomation = static_cast<ACAutomation *>(PyCObject_AsVoidPtr(ac));
    acautomation->match(str, false, results);
    if(results.empty())
    {
        return Py_BuildValue("is", -1, 0);
    }
    Result result = results.front();
    return Py_BuildValue("ls", result.position, result.word);
}

static PyObject* ACAutomation_matchAll(PyObject *self, PyObject* args)
{
    PyObject *ac = 0;
    const char* str;
    std::queue<Result> results;
    int ok = PyArg_ParseTuple(args, "Os", &ac, &str);
    if(!ok) return NULL;
    ACAutomation *acautomation = static_cast<ACAutomation *>(PyCObject_AsVoidPtr(ac));
    acautomation->match(str, true, results);
    PyObject* pList = PyList_New(results.size());
    unsigned long i = 0;
    while(!results.empty())
    {
        Result result = results.front();
        results.pop();
        PyList_SetItem(pList, i++, Py_BuildValue("ls", result.position, result.word));
    }
    return pList;
}

static PyMethodDef Methods[] =
{
     { "new", ACAutomation_new, METH_NOARGS, "create a ACAutomation Object" },
     { "insert", ACAutomation_insert, METH_VARARGS, "insert string" },
     { "build", ACAutomation_build, METH_VARARGS, "build" },
     { "matchOne", ACAutomation_matchOne, METH_VARARGS, "matchOne" },
     { "matchAll", ACAutomation_matchAll, METH_VARARGS, "matchAll" },
     { NULL, NULL, 0, NULL }
};



#if PY_MAJOR_VERSION < 3
    PyMODINIT_FUNC init_ACAutomation(void)
    {
        Py_InitModule( "_ACAutomation", Methods);
    }
#else
    static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_ACAutomation", /* m_name */
        NULL,      /* m_doc */
        -1,                  /* m_size */
        Methods,             /* m_methods */
        NULL,                /* m_reload */
        NULL,                /* m_traverse */
        NULL,                /* m_clear */
        NULL,                /* m_free */
    };

    PyMODINIT_FUNC PyInit__ACAutomation(void)
    {
        PyObject *m = PyModule_Create(&moduledef);
        return m;
    }
#endif