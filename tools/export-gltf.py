import bpy
import sys
import argparse

def get_args():
    # We ignore the arguments before "--" (Blender specific args)
    if "--" in sys.argv:
        args_list = sys.argv[sys.argv.index("--") + 1:]
    else:
        args_list = []

    parser = argparse.ArgumentParser(description="Blender glTF Export Script")
    parser.add_argument("-o", "--output", type=str, help="Output gltf file path", required=True)

    return parser.parse_args(args_list)

def export_gltf():
    args = get_args()

    col_name = "Export"
    filepath = args.output

    # Check if collection exists
    if col_name not in bpy.data.collections:
        print(f"Error: Collection '{col_name}' not found.")
        sys.exit(1)

    # Deselect all and select target collection
    bpy.ops.object.select_all(action='DESELECT')
    export_col = bpy.data.collections[col_name]

    for obj in export_col.objects:
        obj.select_set(True)

    # Ensure there's an active object for the operator
    if export_col.objects:
        bpy.context.view_layer.objects.active = export_col.objects[0]

    # Export Settings
    bpy.ops.export_scene.gltf(
        filepath=filepath,
        use_selection=True,
        export_format='GLTF_SEPARATE',
        export_apply=True,
        export_yup=False,
        export_texcoords=True,
        export_normals=True,
        export_tangents=False
    )
    
    print(f"\n--- SUCCESS: Exported '{col_name}' to '{filepath}' ---\n")

if __name__ == "__main__":
    export_gltf()
