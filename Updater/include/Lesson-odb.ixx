// This file was generated by ODB, object-relational mapping (ORM)
// compiler for C++.
//

namespace odb
{
  // Lesson
  //

  inline
  access::object_traits< ::Lesson >::id_type
  access::object_traits< ::Lesson >::
  id (const object_type& o)
  {
    return o._id;
  }

  inline
  void access::object_traits< ::Lesson >::
  callback (database& db, object_type& x, callback_event e)
  {
    ODB_POTENTIALLY_UNUSED (db);
    ODB_POTENTIALLY_UNUSED (x);
    ODB_POTENTIALLY_UNUSED (e);
  }

  inline
  void access::object_traits< ::Lesson >::
  callback (database& db, const object_type& x, callback_event e)
  {
    ODB_POTENTIALLY_UNUSED (db);
    ODB_POTENTIALLY_UNUSED (x);
    ODB_POTENTIALLY_UNUSED (e);
  }
}

namespace odb
{
  // Lesson
  //

  inline
  void access::object_traits_impl< ::Lesson, id_mysql >::
  erase (database& db, const object_type& obj)
  {
    callback (db, obj, callback_event::pre_erase);
    erase (db, id (obj));
    callback (db, obj, callback_event::post_erase);
  }

  inline
  void access::object_traits_impl< ::Lesson, id_mysql >::
  load_ (statements_type& sts,
         object_type& obj,
         bool)
  {
    ODB_POTENTIALLY_UNUSED (sts);
    ODB_POTENTIALLY_UNUSED (obj);
  }
}

