// Copyright 2015 PsychoLama

#include <fileloader.hpp>
#include <viewport.hpp>

void DefaultFileLoader::save(Viewport* view, const QString& name) {
	// Ouvre le fichier
	QFile file(name);
	file.open(QIODevice::WriteOnly);

	// Crée un flux pour écrire dans le fichier
	QDataStream out(&file);

	// Ecrit le nombre magique dans le fichier (utilisé pour vérifier qu'un fichier est bien un monde Sickle)
	out << static_cast<quint32>(FILE_MAGIC);

	// Ecrit la version du système de fichier dans la sauvegarde
	quint32 revision = staticMetaObject.method(staticMetaObject.indexOfMethod("save")).revision();
	out << revision;

	// Enregistre la version du format de fichier dans la sauvegarde
	auto format = QDataStream::Qt_5_4;
	out << static_cast<qint32>(format);
	out.setVersion(format);

	// Enregistre les propriétés de la caméra
	out << *(view->camera());

	// Enregistre l'ID de la zone courante
	out << static_cast<quint32>(view->world()->currentZoneId());

	// Enregistre le nombre de zones
	auto zones = view->world()->zoneList();
	out << static_cast<quint32>(zones.size());

	// Pour chaque zone
	for (Zone* zone : zones) {
		// Récupère la liste des acteurs
		auto childList = zone->findChildren<Actor*>(QString(), Qt::FindDirectChildrenOnly);

		// Enregistre le nombre d'acteurs
		out << static_cast<quint32>(childList.size());

		// Pour chaque acteur
		for (Actor* obj : childList) {
			// enregistre la classe de celui-ci
			out << QString(obj->metaObject()->className());

			// Et enregistre l'acteur lui même
			out << *obj;
		}
	}
}

void DefaultFileLoader::load(Viewport* view, const QString& name) {
	// Ouvre le fichier
	QFile file(name);
	file.open(QIODevice::ReadOnly);

	// Crée un flux de lecture pour ce fichier
	QDataStream in(&file);

	// Lit le nombre magique et vérifie sa validité
	quint32 magic;
	in >> magic;
	if(magic != FILE_MAGIC) {
		qWarning() << "Bad file format";
		return;
	}

	// Lit la version du système de fichier et vérifie qu'il n'a pas changé
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

	// Charge la version du format de fichier
	qint32 format;
	in >> format;
	in.setVersion(format);

	// Charge les propriétés de la caméra
	in >> *(view->camera());

	// Actie le contexte du viewport
	view->makeCurrent();

	// Charge l'ID de la zone courante
	quint32 curentZone;
	in >> curentZone;

	// Charge le nombre de zones
	quint32 zoneCount;
	in >> zoneCount;

	// Pour chaque zone
	QList<Zone*> zones;
	for (quint32 i = 0; i < zoneCount; i++) {
		// Crée une zone
		auto zone = new Zone(view->world());

		// Lit le nombre d'acteurs de la zone
		quint32 size;
		in >> size;

		// Pour chaque acteur
		for (quint32 i = 0; i < size; i++) {
			// Active le contexte du viewport
			view->makeCurrent();

			// Charge l'acteur
			auto obj = loadObj(&in, zone);

			// Active le signal childAdded du Viewport
			view->childAdded(obj);
		}

		// Ajoute la zone a la liste
		zones.insert(i, zone);
	}

	// Désactive le contexte
	view->doneCurrent();

	// Charge la liste de zones dans le monde
	view->world()->setZoneList(zones);

	// Change l'ID de la zone courante
	view->world()->setCurrentZoneId(curentZone);
}

QDataStream& operator<<(QDataStream& stream, const QObject& obj) {
	// Récupère le méta-objet de l'objet a sauvegarder
	auto metaObject = obj.metaObject();

	// Enregistre le nombre de propriétés
	quint32 count = metaObject->propertyCount();
	stream << count;

	// Pour chaque propriété
	for (quint32 i = 0; i < count; ++i) {
		// Enregistre le nom de la propriété
		auto prop = metaObject->property(i);
		stream << QString(prop.name());

		// Récupère la valeur de la propriété
		auto value = obj.property(prop.name());

		// Enregistre le type de la propriété (objet ou pas)
		bool isObj = static_cast<QMetaType::Type>(prop.type()) == QMetaType::QObjectStar;
		stream << isObj;

		// Si la valeur n'est pas un objet, elle est sauvegardée par valeur
		if (!isObj) {
			stream << value;
		} else {
			// Sinon caste la valeur en QObject
			auto obj = qvariant_cast<QObject*>(value);

			// Enregistre la classe de l'objet
			stream << QString(obj->metaObject()->className());

			// Et l'objet lui même
			stream << *obj;
		}
	}

	// Récupère la liste des acteurs enfants de cet objet
	auto childList = obj.findChildren<Actor*>(QString(), Qt::FindDirectChildrenOnly);

	// Enregistre le nombre d'enfants
	stream << static_cast<quint32>(childList.size());

	// Pour chaque enfant
	for (Actor* obj : childList) {
		// Enregistre sa classe
		stream << QString(obj->metaObject()->className());

		// Et l'enfant lui même
		stream << *obj;
	}

	return stream;
}

QDataStream& operator>>(QDataStream& stream, QObject& obj) {
	// Lit le nombre de propriété de l'objet
	quint32 propCount;
	stream >> propCount;

	// Pour chaque propriété
	for (quint32 i = 0; i < propCount; ++i) {
		// Charge le nom de la propriété
		QString name;
		stream >> name;

		// Charge le type de la propriété (composite ou simple)
		bool isObj;
		stream >> isObj;

		// Si la valeur est simple
		QVariant value;
		if(!isObj) {
			// La charge directement
			stream >> value;
		} else {
			// Sinon charge l'objet
			value = QVariant::fromValue(loadObj(&stream, &obj));
		}

		// Enregistre la propriété dans l'objet
		obj.setProperty(name.toLatin1().data(), value);
	}

	// Charge le nombre d'enfants de l'objet
	quint32 childCount;
	stream >> childCount;

	// Charge les enfants
	for (quint32 i = 0; i < childCount; ++i) {
		loadObj(&stream, &obj);
	}

	return stream;
}

QObject* loadObj(QDataStream* stream, QObject* parent) {
	// Lit la classe de l'ojbet
	QString type;
	(*stream) >> type;

	// Crée un objet a partir du nom de la classe (doit être enregistré dans le système de méta-objets de Qt)
	auto obj = static_cast<QObject*>(QMetaType::create(QMetaType::type(type.toLatin1().data())));

	// Si l'objet a été créé
	if(obj != nullptr) {
		// Si un parent a été défini, active sa methode setParent
		if(parent != nullptr) {
			obj->setParent(parent);
		}

		// Puis charge l'objet et le retourne
		(*stream) >> *obj;
		return obj;
	} else {
		qWarning() << "Error re-creating object of type " << type;
		return nullptr;
	}
}
