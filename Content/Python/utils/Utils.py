import unreal as ue
from typing import List

"""
Finds all materials in asset directory
"""
def FindMaterials(AssetDir) -> List[ue.Material]:
    ue.log("Finding materials in " + AssetDir)
    all_asset_names = ue.EditorAssetLibrary.list_assets(AssetDir, True, False)
    material_assets = []
    for asset_name in all_asset_names:
        loaded_asset = ue.EditorAssetLibrary.load_asset(asset_name)
        if loaded_asset.__class__ == ue.Material:
            ue.log("Found material asset: " + loaded_asset.get_name())
            material_assets.append(loaded_asset)
    return material_assets

"""
Finds all static meshes in asset directory
"""
def FindStaticMeshes(AssetDir) -> List[ue.StaticMesh]:
    ue.log("Finding static meshes in " + AssetDir)
    all_asset_names = ue.EditorAssetLibrary.list_assets(AssetDir, True, False)
    sm_assets = []
    for asset_name in all_asset_names:
        loaded_asset = ue.EditorAssetLibrary.load_asset(asset_name)
        if loaded_asset.__class__ == ue.StaticMesh:
            ue.log("Found static mesh asset: " + loaded_asset.get_name())
            sm_assets.append(loaded_asset)
    return sm_assets

"""
Aims to find all materials that are not suitable for nanite meshes

Issues: once a borked asset is loaded in editor it's assigned the default asset WorldGridMaterial
"""
def GetNonNaniteMaterialUsedByNaniteMeshes(AssetDir) -> dict[str, List[str]]:
    nanite_lookup = dict();

    static_meshes = FindStaticMeshes(AssetDir)
    for sm in static_meshes:
        nanite_settings = sm.get_editor_property("nanite_settings")
        if nanite_settings.enabled == True:
            for m_idx in range(0, sm.get_num_sections(0)):
                material = sm.get_material(m_idx)
                ue.log(material.__class__)
                if material.__class__ == ue.MaterialInstanceConstant:
                    continue
                if material.__class__ == ue.MaterialInstance:
                    material = material.get_base_material()
                if material.get_editor_property("used_with_nanite") == False:
                    ue.log("Found non-nanite material: " + material.get_name() + " for nanite mesh: " + sm.get_name())
                    nanite_lookup[material.get_name()].append(sm.get_name()) 
    return nanite_lookup

"""
WIP - Aims to correct materials and meshes for mismatch
"""
def FixMaterialsForNaniteAndLumen(AssetDir):
    # translucent is not supported by nanite
    # retain additive and masked modes by disabling nanite on referenced meshes
    IgnoreBlendModesForNanite = {
        ue.BlendMode.BLEND_TRANSLUCENT: True,
        ue.BlendMode.BLEND_ADDITIVE: True,
        ue.BlendMode.BLEND_MASKED: True
    }

    for material in FindMaterials(AssetDir):
        ignore_blend_mode = False
        material_name = material.get_name()
        blendmode_name = str(material.get_blend_mode().get_display_name())
        enabled_properties = [
            "used_with_instanced_static_meshes",
            "used_with_nanite"
        ]
        # check that properties are supported
        for property in enabled_properties:
            if material.get_editor_property(property) == False:
                if property == "used_with_nanite" and IgnoreBlendModesForNanite.get(material.blend_mode, False):
                    ue.log("Not enabling nanite on material: " + material_name + " with blend mode: " + blendmode_name)
                    ignore_blend_mode = True
                    for ref in ue.EditorAssetLibrary.find_package_referencers_for_asset(material.get_path_name(), load_assets_to_confirm=False):
                        ue.log("Material: " + material_name + " referenced by: " + ref)
                    continue
                ue.log("Enabling property: " + property + " on material: " + material.get_name())
                material.set_editor_property(property, True)

        # skip blend mode check for blend modes to ignore
        if ignore_blend_mode == True:
            ue.log("Skipping blend mode check for: " + material_name)
            continue

        # check that blend mode is supported
        if material.blend_mode != ue.BlendMode.BLEND_OPAQUE:
            if IgnoreBlendModesForNanite.get(material.blend_mode, False) and material.get_editor_property("used_with_nanite") == True:
                ue.log("Disabling nanite on material: " + material_name + " with blend mode: " + blendmode_name)
                material.set_editor_property("used_with_nanite", False)
            else:
                ue.log("Material: " + material.get_name() + " has invalid blend mode for Nanite: " + blendmode_name + ", setting to opaque")
                material.blend_mode == ue.BlendMode.BLEND_OPAQUE

        ue.log("Saving material asset: " + material.get_name())
        ue.EditorAssetLibrary.save_loaded_asset(material, only_if_is_dirty=False)
        ue.EditorAssetLibrary.save_asset(material.get_path_name(), only_if_is_dirty=False)


