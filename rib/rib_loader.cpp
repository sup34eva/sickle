#include "rib_loader.hpp"
#include <viewport.hpp>
#include <cube.hpp>
#include <sphere.hpp>
#include <ri.hpp>

void RIBFileLoader::save(Viewport* view, const QString& name) {
	RiBegin(name.toLatin1().data()); {
		RiDisplay((name.split('.')[0] + ".tif").toLatin1().data(), RI_FRAMEBUFFER, RI_RGB, RI_NULL);
		RiFormat(600, 400, 1);

		auto cam = view->camera();
		auto pos = cam->position();
		auto rot = cam->orientation().normalized();
		RtFloat fov = 45.0f;
		RiProjection(RI_PERSPECTIVE, RI_FOV, &fov, RI_NULL);
		RiTranslate(pos.x(), pos.y(), pos.z());
		RiRotate(rot.scalar(), rot.x(), rot.y(), rot.z());

		RiWorldBegin(); {
			RtString lightName = "ambient";
			RtFloat lightInt = 0.6f;
			RiLightSource(RI_AMBIENTLIGHT, RI_HANDLEID, &lightName, RI_INTENSITY, &lightInt, RI_NULL);
			RiSurface(RI_PLASTIC, RI_NULL);

			auto list = view->children();
			for(auto child : list) {
				auto actor = dynamic_cast<Actor*>(child);
				if(actor != nullptr) {
					RiAttributeBegin(); {
						auto pos = actor->position();
						RiTranslate(pos.x(), pos.y(), pos.z());

						auto rot = actor->orientation().normalized();
						RiRotate(rot.scalar(), rot.x(), rot.y(), rot.z());

						auto scale = actor->scale();
						RiScale(scale.x(), scale.y(), scale.z());

						auto sphere = dynamic_cast<Sphere*>(actor);
						if(sphere) {
							QColor col = qvariant_cast<QColor>(sphere->colors().first());
							RtColor color = {col.redF(), col.greenF(), col.blueF()};
							RiColor(color);

							RiSphere(1, -1, 1, 360, RI_NULL);
						}

						auto cube = dynamic_cast<Cube*>(actor);
						if(cube) {
							// TODO
						}
					} RiAttributeEnd();
				}
			}
		} RiWorldEnd();

	} RiEnd();
}

void RIBFileLoader::load(Viewport* view, const QString& name) {
	Q_UNUSED(view);
	Q_UNUSED(name);
	// TODO
}
