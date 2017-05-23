#pragma once


#include "assimp/Importer.hpp"
#include "engine/array.h"
#include "engine/associative_array.h"
#include "engine/binary_array.h"
#include "engine/mt/sync.h"
#include "engine/string.h"
#include "engine/lumix.h"
#include "editor/studio_app.h"


struct aiAnimation;
struct aiMaterial;
struct aiMesh;
struct lua_State;
class Metadata;


namespace Lumix
{


class WorldEditor;
namespace MT { class Task; }



struct ImportTexture
{
	struct aiTexture* texture;
	char path[MAX_PATH_LENGTH];
	char src[MAX_PATH_LENGTH];
	bool import;
	bool to_dds;
	bool is_valid;
};


struct ImportMaterial
{
	const struct aiScene* scene;
	aiMaterial* material;
	bool import;
	bool alpha_cutout;
	int texture_count;
	char shader[20];
	char name[128];
	ImportTexture textures[16];
};


struct ImportAnimation
{
	bool import;
	aiAnimation* animation;
	int root_motion_bone_idx = -1;
	const aiScene* scene;
	char output_filename[MAX_PATH_LENGTH];
};


struct ImportMesh
{
	ImportMesh(IAllocator& allocator)
		: map_to_input(allocator)
		, map_from_input(allocator)
		, indices(allocator)
	{
	}

	int lod;
	bool import;
	bool import_physics;
	aiMesh* mesh;
	const aiScene* scene;
	int material;
	Array<unsigned int> map_to_input;
	Array<unsigned int> map_from_input;
	Array<i32> indices;
};



class ImportAssetDialog LUMIX_FINAL : public StudioApp::IPlugin
{
	friend struct ImportTask;
	friend struct ConvertTask;
	friend struct ImportTextureTask;
	public:
		enum Orientation : int
		{
			Y_UP,
			Z_UP,
			Z_MINUS_UP,
			X_MINUS_UP
		};

		struct DDSConvertCallbackData
		{
			ImportAssetDialog* dialog;
			const char* dest_path;
			bool cancel_requested;
		};

	public:
		ImportAssetDialog(StudioApp& app);
		~ImportAssetDialog();
		void setMessage(const char* message);
		void setImportMessage(const char* message, float progress_fraction);
		WorldEditor& getEditor() { return m_editor; }
		void onWindowGUI() override;
		DDSConvertCallbackData& getDDSConvertCallbackData() { return m_dds_convert_callback; }
		int importAsset(lua_State* L);
		const char* getName() const override { return "import_asset"; }

	public:
		bool m_is_opened;

	private:
		bool checkSource();
		void checkTask(bool wait);
		void convert(bool use_ui);
		void import();
		void getMessage(char* msg, int max_size);
		bool hasMessage();
		void importTexture();
		bool isTextureDirValid() const;
		void onMaterialsGUI();
		void onMeshesGUI();
		void onAnimationsGUI();
		void onImageGUI();
		void onLODsGUI();
		void onAction();
		void saveModelMetadata();
		bool isOpened() const;
		void clearSources();
		void addSource(const char* src);

	public:
		WorldEditor& m_editor;
		Array<u32> m_saved_textures;
		Array<Assimp::Importer> m_importers;
		Array<StaticString<MAX_PATH_LENGTH> > m_sources;
		Array<ImportAnimation> m_animations;
		Array<ImportMesh> m_meshes;
		Array<ImportMaterial> m_materials;
		char m_import_message[1024];
		
		struct ImageData
		{
			u8* data;
			int width;
			int height;
			int comps;
			int resize_size[2];
		} m_image;
		
		struct ModelData
		{
			float mesh_scale;
			float lods[4];
			bool create_billboard_lod;
			bool optimize_mesh_on_import;
			bool gen_smooth_normal;
			bool remove_doubles;
			bool center_meshes;
			Orientation orientation;
			Orientation root_orientation;
			bool make_convex;
			bool import_vertex_colors;
			bool all_nodes;
			float position_error;
			float rotation_error;
			float time_scale;
		} m_model;

		float m_progress_fraction;
		char m_message[1024];
		char m_last_dir[MAX_PATH_LENGTH];
		char m_source[MAX_PATH_LENGTH];
		char m_mesh_output_filename[MAX_PATH_LENGTH];
		char m_output_dir[MAX_PATH_LENGTH];
		char m_texture_output_dir[MAX_PATH_LENGTH];
		bool m_convert_to_dds;
		bool m_convert_to_raw;
		bool m_is_normal_map;
		bool m_is_converting;
		bool m_is_importing;
		bool m_is_importing_texture;
		float m_raw_texture_scale;
		MT::Task* m_task;
		MT::SpinMutex m_mutex;
		Metadata& m_metadata;
		DDSConvertCallbackData m_dds_convert_callback;
};


} // namespace Lumix