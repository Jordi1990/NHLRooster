// This file was generated by ODB, object-relational mapping (ORM)
// compiler for C++.
//

#ifndef CHANGE_ITEM_ODB_HXX
#define CHANGE_ITEM_ODB_HXX

#include <odb/version.hxx>

#if (ODB_VERSION != 20400UL)
#error ODB runtime version mismatch
#endif

#include <odb/pre.hxx>

#include "ChangeItem.h"

#include <memory>
#include <cstddef>

#include <odb/core.hxx>
#include <odb/traits.hxx>
#include <odb/callback.hxx>
#include <odb/wrapper-traits.hxx>
#include <odb/pointer-traits.hxx>
#include <odb/container-traits.hxx>
#include <odb/no-op-cache-traits.hxx>
#include <odb/result.hxx>
#include <odb/simple-object-result.hxx>

#include <odb/details/unused.hxx>
#include <odb/details/shared-ptr.hxx>

namespace odb
{
  // ChangeItem
  //
  template <>
  struct class_traits< ::ChangeItem >
  {
    static const class_kind kind = class_object;
  };

  template <>
  class access::object_traits< ::ChangeItem >
  {
    public:
    typedef ::ChangeItem object_type;
    typedef ::ChangeItem* pointer_type;
    typedef odb::pointer_traits<pointer_type> pointer_traits;

    static const bool polymorphic = false;

    typedef long unsigned int id_type;

    static const bool auto_id = true;

    static const bool abstract = false;

    static id_type
    id (const object_type&);

    typedef
    no_op_pointer_cache_traits<pointer_type>
    pointer_cache_traits;

    typedef
    no_op_reference_cache_traits<object_type>
    reference_cache_traits;

    static void
    callback (database&, object_type&, callback_event);

    static void
    callback (database&, const object_type&, callback_event);
  };
}

#include <odb/details/buffer.hxx>

#include <odb/mysql/version.hxx>
#include <odb/mysql/forward.hxx>
#include <odb/mysql/binding.hxx>
#include <odb/mysql/mysql-types.hxx>
#include <odb/mysql/query.hxx>

namespace odb
{
  // ChangeItem
  //
  template <typename A>
  struct query_columns< ::ChangeItem, id_mysql, A >
  {
    // id
    //
    typedef
    mysql::query_column<
      mysql::value_traits<
        long unsigned int,
        mysql::id_ulonglong >::query_type,
      mysql::id_ulonglong >
    id_type_;

    static const id_type_ id;

    // title
    //
    typedef
    mysql::query_column<
      mysql::value_traits<
        ::std::string,
        mysql::id_string >::query_type,
      mysql::id_string >
    title_type_;

    static const title_type_ title;

    // description
    //
    typedef
    mysql::query_column<
      mysql::value_traits<
        ::std::string,
        mysql::id_string >::query_type,
      mysql::id_string >
    description_type_;

    static const description_type_ description;

    // teacher
    //
    typedef
    mysql::query_column<
      mysql::value_traits<
        ::std::string,
        mysql::id_string >::query_type,
      mysql::id_string >
    teacher_type_;

    static const teacher_type_ teacher;

    // location
    //
    typedef
    mysql::query_column<
      mysql::value_traits<
        ::std::string,
        mysql::id_string >::query_type,
      mysql::id_string >
    location_type_;

    static const location_type_ location;

    // startdate
    //
    typedef
    mysql::query_column<
      mysql::value_traits<
        ::std::string,
        mysql::id_string >::query_type,
      mysql::id_string >
    startdate_type_;

    static const startdate_type_ startdate;

    // enddate
    //
    typedef
    mysql::query_column<
      mysql::value_traits<
        ::std::string,
        mysql::id_string >::query_type,
      mysql::id_string >
    enddate_type_;

    static const enddate_type_ enddate;

    // year
    //
    typedef
    mysql::query_column<
      mysql::value_traits<
        ::std::string,
        mysql::id_string >::query_type,
      mysql::id_string >
    year_type_;

    static const year_type_ year;

    // type
    //
    typedef
    mysql::query_column<
      mysql::value_traits<
        ::std::string,
        mysql::id_string >::query_type,
      mysql::id_string >
    type_type_;

    static const type_type_ type;

    // change
    //
    typedef
    mysql::query_column<
      mysql::value_traits<
        ::std::string,
        mysql::id_string >::query_type,
      mysql::id_string >
    change_type_;

    static const change_type_ change;

    // weekNr
    //
    typedef
    mysql::query_column<
      mysql::value_traits<
        int,
        mysql::id_long >::query_type,
      mysql::id_long >
    weekNr_type_;

    static const weekNr_type_ weekNr;
  };

  template <typename A>
  const typename query_columns< ::ChangeItem, id_mysql, A >::id_type_
  query_columns< ::ChangeItem, id_mysql, A >::
  id (A::table_name, "`id`", 0);

  template <typename A>
  const typename query_columns< ::ChangeItem, id_mysql, A >::title_type_
  query_columns< ::ChangeItem, id_mysql, A >::
  title (A::table_name, "`title`", 0);

  template <typename A>
  const typename query_columns< ::ChangeItem, id_mysql, A >::description_type_
  query_columns< ::ChangeItem, id_mysql, A >::
  description (A::table_name, "`description`", 0);

  template <typename A>
  const typename query_columns< ::ChangeItem, id_mysql, A >::teacher_type_
  query_columns< ::ChangeItem, id_mysql, A >::
  teacher (A::table_name, "`teacher`", 0);

  template <typename A>
  const typename query_columns< ::ChangeItem, id_mysql, A >::location_type_
  query_columns< ::ChangeItem, id_mysql, A >::
  location (A::table_name, "`location`", 0);

  template <typename A>
  const typename query_columns< ::ChangeItem, id_mysql, A >::startdate_type_
  query_columns< ::ChangeItem, id_mysql, A >::
  startdate (A::table_name, "`startdate`", 0);

  template <typename A>
  const typename query_columns< ::ChangeItem, id_mysql, A >::enddate_type_
  query_columns< ::ChangeItem, id_mysql, A >::
  enddate (A::table_name, "`enddate`", 0);

  template <typename A>
  const typename query_columns< ::ChangeItem, id_mysql, A >::year_type_
  query_columns< ::ChangeItem, id_mysql, A >::
  year (A::table_name, "`year`", 0);

  template <typename A>
  const typename query_columns< ::ChangeItem, id_mysql, A >::type_type_
  query_columns< ::ChangeItem, id_mysql, A >::
  type (A::table_name, "`type`", 0);

  template <typename A>
  const typename query_columns< ::ChangeItem, id_mysql, A >::change_type_
  query_columns< ::ChangeItem, id_mysql, A >::
  change (A::table_name, "`change`", 0);

  template <typename A>
  const typename query_columns< ::ChangeItem, id_mysql, A >::weekNr_type_
  query_columns< ::ChangeItem, id_mysql, A >::
  weekNr (A::table_name, "`weekNr`", 0);

  template <typename A>
  struct pointer_query_columns< ::ChangeItem, id_mysql, A >:
    query_columns< ::ChangeItem, id_mysql, A >
  {
  };

  template <>
  class access::object_traits_impl< ::ChangeItem, id_mysql >:
    public access::object_traits< ::ChangeItem >
  {
    public:
    struct id_image_type
    {
      unsigned long long id_value;
      my_bool id_null;

      std::size_t version;
    };

    struct image_type
    {
      // _id
      //
      unsigned long long _id_value;
      my_bool _id_null;

      // _title
      //
      details::buffer _title_value;
      unsigned long _title_size;
      my_bool _title_null;

      // _description
      //
      details::buffer _description_value;
      unsigned long _description_size;
      my_bool _description_null;

      // _teacher
      //
      details::buffer _teacher_value;
      unsigned long _teacher_size;
      my_bool _teacher_null;

      // _location
      //
      details::buffer _location_value;
      unsigned long _location_size;
      my_bool _location_null;

      // _startdate
      //
      details::buffer _startdate_value;
      unsigned long _startdate_size;
      my_bool _startdate_null;

      // _enddate
      //
      details::buffer _enddate_value;
      unsigned long _enddate_size;
      my_bool _enddate_null;

      // _year
      //
      details::buffer _year_value;
      unsigned long _year_size;
      my_bool _year_null;

      // _type
      //
      details::buffer _type_value;
      unsigned long _type_size;
      my_bool _type_null;

      // _change
      //
      details::buffer _change_value;
      unsigned long _change_size;
      my_bool _change_null;

      // _weekNr
      //
      int _weekNr_value;
      my_bool _weekNr_null;

      std::size_t version;
    };

    struct extra_statement_cache_type;

    using object_traits<object_type>::id;

    static id_type
    id (const id_image_type&);

    static id_type
    id (const image_type&);

    static bool
    grow (image_type&,
          my_bool*);

    static void
    bind (MYSQL_BIND*,
          image_type&,
          mysql::statement_kind);

    static void
    bind (MYSQL_BIND*, id_image_type&);

    static bool
    init (image_type&,
          const object_type&,
          mysql::statement_kind);

    static void
    init (object_type&,
          const image_type&,
          database*);

    static void
    init (id_image_type&, const id_type&);

    typedef mysql::object_statements<object_type> statements_type;

    typedef mysql::query_base query_base_type;

    static const std::size_t column_count = 11UL;
    static const std::size_t id_column_count = 1UL;
    static const std::size_t inverse_column_count = 0UL;
    static const std::size_t readonly_column_count = 0UL;
    static const std::size_t managed_optimistic_column_count = 0UL;

    static const std::size_t separate_load_column_count = 0UL;
    static const std::size_t separate_update_column_count = 0UL;

    static const bool versioned = false;

    static const char persist_statement[];
    static const char find_statement[];
    static const char update_statement[];
    static const char erase_statement[];
    static const char query_statement[];
    static const char erase_query_statement[];

    static const char table_name[];

    static void
    persist (database&, object_type&);

    static pointer_type
    find (database&, const id_type&);

    static bool
    find (database&, const id_type&, object_type&);

    static bool
    reload (database&, object_type&);

    static void
    update (database&, const object_type&);

    static void
    erase (database&, const id_type&);

    static void
    erase (database&, const object_type&);

    static result<object_type>
    query (database&, const query_base_type&);

    static unsigned long long
    erase_query (database&, const query_base_type&);

    public:
    static bool
    find_ (statements_type&,
           const id_type*);

    static void
    load_ (statements_type&,
           object_type&,
           bool reload);
  };

  template <>
  class access::object_traits_impl< ::ChangeItem, id_common >:
    public access::object_traits_impl< ::ChangeItem, id_mysql >
  {
  };

  // ChangeItem
  //
}

#include "ChangeItem-odb.ixx"

#include <odb/post.hxx>

#endif // CHANGE_ITEM_ODB_HXX