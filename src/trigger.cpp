#include <trigger.hpp>
#include <cube.hpp>

template<>
int Trigger::tBase::s_instances = 0;

Trigger::Trigger(QObject* parent) : Geometry(parent) {
	setObjectName(QString(tr("Trigger %1")).arg(Trigger::tBase::s_instances));
	colors({
		QColor(255, 0, 0)
	});
}

void Trigger::draw(const DrawInfo &info) {
	auto lineInfo = info;
	lineInfo.mode = GL_LINES;
	tBase::draw(lineInfo);
}

QVector<GLfloat> calcTriggerColors() {
	QVector<GLfloat> colors;
	colors.reserve(6 * 4);

	for (GLfloat face = 0; face < 6; face++) for (int vert = 0; vert < 4; vert++) {
		colors.append(0.0f);
	}

	return colors;
}

template<>
QOpenGLVertexArrayObject* Trigger::tBase::s_vao = nullptr;
template<>
ProgramList Trigger::tBase::s_programList = {};
template<>
QHash<QString, QOpenGLBuffer*> Trigger::tBase::s_buffers = {};

template<>
QVector<quint32> Trigger::tBase::s_indexBuffer = calcCubeIndices();

template<>
QHash<QString, QVector<GLfloat>> Trigger::tBase::s_buffersData = {
	{"Position", {
		-1.0f, -1.0f, -1.0f,		// Face 1
		-1.0f, -1.0f,  1.0f,		//
		-1.0f,  1.0f,  1.0f,		//
		-1.0f,  1.0f, -1.0f,		//
		 1.0f, -1.0f, -1.0f,		// Face 2
		 1.0f,  1.0f, -1.0f,		//
		 1.0f,  1.0f,  1.0f,		//
		 1.0f, -1.0f,  1.0f,		//
		-1.0f, -1.0f, -1.0f,		// Face 3
		 1.0f, -1.0f, -1.0f,		//
		 1.0f, -1.0f,  1.0f,		//
		-1.0f, -1.0f,  1.0f,		//
		-1.0f,  1.0f, -1.0f,		// Face 4
		-1.0f,  1.0f,  1.0f,		//
		 1.0f,  1.0f,  1.0f,		//
		 1.0f,  1.0f, -1.0f,		//
		-1.0f, -1.0f, -1.0f,		// Face 5
		-1.0f,  1.0f, -1.0f,		//
		 1.0f,  1.0f, -1.0f,		//
		 1.0f, -1.0f, -1.0f,		//
		-1.0f, -1.0f,  1.0f,		// Face 6
		 1.0f, -1.0f,  1.0f,		//
		 1.0f,  1.0f,  1.0f,		//
		-1.0f,  1.0f,  1.0f,		//
	}},
	{"Color", calcTriggerColors()},
	{"UV", calcCubeUVs()}
};
