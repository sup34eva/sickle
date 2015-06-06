#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <QObject>
#include <globals.hpp>

/*! \brief Définit les propriétés d'un materiau
 *
 * Cette structure contient tous les paramètres passés au shader pour définir un materiau, basé sur le principe des
 * shaders Disney.
 */
class Material : public QObject {
	Q_OBJECT

	public:
		Material(QObject* parent = nullptr);

		prop(float, metallic);  //! Si la surface est refléchissante (ex: metal)
		prop(float, subsurface);  //! Si la surface diffuse la lumière (ex: peau, bougie)
		prop(float, specular);  //! Si la surface est brillante (ex: plastique)
		prop(float, roughness);  //! Si la surface est rugeuse (ex: bois, tissu)
		prop(float, specularTint);  //! Si la composante speculaire doit prendre la couleur de la surface
		prop(float, anisotropic);  //! Si la reflection est anisotropique (ex: soie, cheveux)
		prop(float, sheen);  //! Si la surface a un eclat supplémentaire (ex: tissu)
		prop(float, sheenTint);  //! Si l'eclat doit prendre la teinte de la surface
		prop(float, clearcoat);  //! Si la surface est vernie
		prop(float, clearcoatGloss);  //! Si le vernis est brillant
};

#endif // MATERIAL_HPP
