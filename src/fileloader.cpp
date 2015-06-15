// Copyright 2015 PsychoLama

#include <fileloader.hpp>
#include <viewport.hpp>

void DefaultFileLoader::save(Viewport* view, const QString& name) {
	QFile file(name);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out << static_cast<quint32>(FILE_MAGIC);  // Magic number
	quint32 revision = staticMetaObject.method(staticMetaObject.indexOfMethod("save")).revision();
	out << revision;  // Sickle version

	auto format = QDataStream::Qt_5_4;  // File format
	out << static_cast<qint32>(format);
	out.setVersion(format);

	// Data
	out << *(view->camera());

	out << static_cast<quint32>(view->world()->currentZoneId());

	auto zones = view->world()->zoneList();
	out << static_cast<quint32>(zones.size());
	for (Zone* zone : zones) {
		auto childList = zone->findChildren<Actor*>(QString(), Qt::FindDirectChildrenOnly);
		out << static_cast<quint32>(childList.size());
		for (Actor* obj : childList) {
			out << QString(obj->metaObject()->className());
			out << *obj;
		}
	}
}

void DefaultFileLoader::load(Viewport* view, const QString& name) {
	QFile file(name);
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);

	quint32 magic;
	in >> magic;  // Magic number
	if(magic != FILE_MAGIC) {
		qWarning() << "Bad file format";
		return;
	}

	quint32 revision = staticMetaObject.method(staticMetaObject.indexOfMethod("save")).revision();
	quint32 version;
	in >> version;
	if(version < revision) {
		qWarning() << "Older file format";
		return;
	} else if (version > revision) {
		qWarning() << "Newer file format";
		return;
	}

	qint32 format;  // File format
	in >> format;
	in.setVersion(format);

	QList<Zone*> zones;

	// Data
	in >> *(view->camera());

	view->makeCurrent();

	quint32 curentZone;
	in >> curentZone;

	quint32 zoneCount;
	in >> zoneCount;
	for (quint32 i = 0; i < zoneCount; i++) {
		auto zone = new Zone(view->world());

		quint32 size;
		in >> size;
		for (quint32 i = 0; i < size; i++) {
			view->makeCurrent();
			auto obj = loadObj(&in, zone);
			view->childAdded(obj);
		}

		zones.insert(i, zone);
	}

	view->doneCurrent();
	view->world()->setZoneList(zones);
	view->world()->setCurrentZoneId(curentZone);
}

QDataStream& operator<<(QDataStream& stream, const QObject& obj) {
	auto metaObject = obj.metaObject();

	quint32 count = metaObject->propertyCount();
	stream << count;

	for (quint32 i = 0; i < count; ++i) {
		auto prop = metaObject->property(i);
		stream << QString(prop.name());
		auto value = obj.property(prop.name());
		bool isObj = static_cast<QMetaType::Type>(prop.type()) == QMetaType::QObjectStar;
		stream << isObj;
		if (!isObj) {
			stream << value;
		} else {
			auto obj = qvariant_cast<QObject*>(value);
			stream << QString(obj->metaObject()->className());
			stream << *obj;
		}
	}

	auto childList = obj.findChildren<Actor*>(QString(), Qt::FindDirectChildrenOnly);
	stream << static_cast<quint32>(childList.size());

	for (Actor* obj : childList) {
		stream << QString(obj->metaObject()->className());
		stream << *obj;
	}

	return stream;
}

QDataStream& operator>>(QDataStream& stream, QObject& obj) {
	quint32 propCount;
	stream >> propCount;

	for (quint32 i = 0; i < propCount; ++i) {
		QString name;
		stream >> name;
		bool isObj;
		stream >> isObj;
		QVariant value;
		if(!isObj) {
			stream >> value;
		} else {
			value = QVariant::fromValue(loadObj(&stream, &obj));
		}
		obj.setProperty(name.toLatin1().data(), value);
	}

	quint32 childCount;
	stream >> childCount;

	for (quint32 i = 0; i < childCount; ++i) {
		loadObj(&stream, &obj);
	}

	return stream;
}

QObject* loadObj(QDataStream* stream, QObject* parent) {
	QString type;
	(*stream) >> type;
	auto obj = static_cast<QObject*>(QMetaType::create(QMetaType::type(type.toLatin1().data())));
	if(obj != nullptr) {
		if(parent != nullptr) {
			obj->setParent(parent);
		}
		(*stream) >> *obj;
		return obj;
	} else {
		qWarning() << "Error re-creating object of type " << type;
		return nullptr;
	}
}
