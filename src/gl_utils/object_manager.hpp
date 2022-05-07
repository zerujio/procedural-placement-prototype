#ifndef GL_UTILS_OBJECT_MANAGER_HPP
#define GL_UTILS_OBJECT_MANAGER_HPP

namespace GL {

  template<class Object>
  class ObjectManager {

  public:
    template<class... Args>
    ObjectManager(Args... args) {
      Object::create(args);
    }

    ~ObjectManager() {
      O
    }

    Object

  private:
    Object m_id;
  };

} // namespace GL

#endif//GL_UTILS_OBJECT_MANAGER_HPP
