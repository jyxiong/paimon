#pragma once

#include <entt/entt.hpp>

namespace paimon::ecs
{
	class Scene;

	/// @brief An entity represents any object in a scene
	/// @note This class is ment to be passed by value since its just an id
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entityHandle, Scene* scene);
		Entity(const Entity&) = default;
		Entity(Entity&&) = default;

		auto operator=(const Entity&) -> Entity& = default;
		auto operator=(Entity&&) -> Entity& = default;

		bool operator==(const Entity& other) const;
		bool operator!=(const Entity& other) const;
		
		operator bool() const { return m_id != entt::null; }
		operator entt::entity() const { return m_id; }
		operator uint32_t() const { return static_cast<uint32_t>(m_id); }

		[[nodiscard]] auto registry() const -> entt::registry&;

		/// @brief Checks if the entity has all components of type T...
		template<typename... T>
		auto has() const -> bool
		{
			return registry().all_of<T...>(m_id);
		}

		/// @brief Acces to the component of type T
		template<typename T>
		auto get() const -> T&
		{
			return registry().get<T>(m_id);
		}

		/// @brief Adds a component of type T to the entity
		/// @return A refrence to the new component
		template<typename T, typename... Args>
		auto add(Args&&... args) -> T&
		{
			return registry().emplace<T>(m_id, std::forward<Args>(args)...);
		}

		template<typename T>
		auto getOrAdd() -> T&
		{
			return registry().get_or_emplace<T>(m_id);
		}

		/// @brief Removes a component of type T from the entity
		/// @note Entity MUST have the component and it MUST be an optional component
		template<typename T>
			requires OptionalComponent<T>
		void remove()
		{
			registry().remove<T>(m_id);
		}

		void setParent(Entity parent);
		void removeParent();
		void addChild(Entity child);
		void removeChild(Entity child);
		void removeChildren();

	private:

		entt::entity m_id{ entt::null };
		Scene* m_scene{ nullptr };
	};
}
