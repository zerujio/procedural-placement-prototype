#ifndef GL_UTILS_OBJECT_MANAGER_HPP
#define GL_UTILS_OBJECT_MANAGER_HPP

namespace GL {

template<class Object>
class ObjectManager {

public:
    template<class... Args>
    ObjectManager(Args... args) {
        m_handle = Object::create(args...);
    }

    ~ObjectManager() {
        Object::destroy(m_handle);
    }

    ObjectManager(ObjectManager &&other) noexcept
            : m_handle(other) {
        other.m_handle = Object();
    }

    ObjectManager &operator=(ObjectManager &&other) noexcept {
        Object::destroy(m_handle);
        m_handle = other.m_handle;
        other.m_handle = Object();
    }

    Object handle() const {
        return m_handle;
    }

private:
    Object m_handle;
};

} // namespace GL

#endif//GL_UTILS_OBJECT_MANAGER_HPP
