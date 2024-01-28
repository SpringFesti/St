#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <SQLiteCpp/SQLiteCpp.h>

int main(int argc, char** argv)
{
    SQLite::Database db("st.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) ;
    SQLite::Transaction init_tra(db) ;
    SQLite::Statement init(db, "CREATE TABLE IF NOT EXISTS students (name Varchar(20), gender Varchar(10), birthday Varchar(30), hobby Text, mb Varchar(20), qq Varchar(20));") ;
    init.executeStep() ;
    init_tra.commit() ;

    nana::appearance view ;
    view.decoration = false ;
    nana::form main_form(nana::API::make_center(512, 768)) ;
    nana::form view_form(nana::API::make_center(512, 256), view) ;
    main_form.caption("主界面") ;
    view_form.caption("编辑界面") ;

    view_form.events().unload([&](){
        view_form.hide() ;
    }) ;

    main_form.div("vert <weight=95% list><<add> <edit> <delete> <export> <import> >") ;
    view_form.div("vert < <name> <gender> <birthday> <hobby>> <mb> <qq> <weight=20 <> <save> <cancel> <>>") ;

    nana::listbox st_list(main_form) ;
    nana::button st_add(main_form) ;
    nana::button st_edit(main_form) ;
    nana::button st_del(main_form) ;
    nana::button st_import(main_form) ;
    nana::button st_export(main_form) ;
    st_add.caption("新增") ;
    st_edit.caption("查看") ;
    st_del.caption("删除") ;
    st_import.caption("导入") ;
    st_export.caption("导出此项") ;

    nana::textbox st_name(view_form) ;
    nana::textbox st_gender(view_form) ;
    nana::textbox st_birthday(view_form) ;
    nana::textbox st_hobby(view_form) ;
    nana::textbox st_mb(view_form) ;
    nana::textbox st_qq(view_form) ;
    nana::button st_save(view_form) ;
    nana::button st_cancel(view_form) ;
    st_list.enable_single(true, true) ;
    st_name.tip_string("姓名") ;
    st_gender.tip_string("性别") ;
    st_birthday.tip_string("生日") ;
    st_hobby.tip_string("爱好") ;
    st_mb.tip_string("手机号码") ;
    st_qq.tip_string("QQ") ;
    st_save.caption("保存") ;
    st_cancel.caption("取消") ;

    st_list.append_header("姓名") ;
    auto cat = st_list.at(0) ;


    SQLite::Statement query_all(db, "SELECT name FROM students") ;
    while(query_all.executeStep()) {
        auto name = query_all.getColumn("name").getString() ;
        cat.append({name}) ;
    }

    main_form["list"] << st_list ;
    main_form["add"] << st_add ;
    main_form["edit"] << st_edit ;
    main_form["delete"] << st_del ;
    //main_form["export"] << st_export ;
    //main_form["import"] << st_import ;
    main_form.collocate() ;

    view_form["name"] << st_name ;
    view_form["gender"] << st_gender ;
    view_form["birthday"] << st_birthday ;
    view_form["hobby"] << st_hobby ;
    view_form["mb"] << st_mb ;
    view_form["qq"] << st_qq ;
    view_form["save"] << st_save ;
    view_form["cancel"] << st_cancel ;
    
    view_form.collocate() ;

    main_form.show() ;
    //view_form.show() ;

    bool new_st ;
    std::string opt_name ;


    main_form.events().destroy([&](){
        view_form.close() ;
    }) ;

    st_add.events().click([&](){
        new_st = true ;
        view_form.show() ;
    }) ;

    st_edit.events().click([&](){
        
        if(!st_list.selected().empty()) {
            auto kp = st_list.selected().front() ;
            opt_name = st_list.at(kp).text(0) ;

            SQLite::Statement query(db, "Select * from students where name = ?") ;
            query.bind(1, opt_name) ;
            if(query.executeStep()) {
                auto name = query.getColumn("name").getString() ;
                auto gender = query.getColumn("gender").getString() ;
                auto birthday = query.getColumn("birthday").getString() ;
                auto hobby = query.getColumn("hobby").getString() ;
                auto mb = query.getColumn("mb").getString() ;
                auto qq = query.getColumn("qq").getString() ;

                st_name.caption(name) ;
                st_gender.caption(gender) ;
                st_birthday.caption(birthday) ;
                st_hobby.caption(hobby) ;
                st_mb.caption(mb) ;
                st_qq.caption(qq) ;
            }
        
            new_st = false ;
            view_form.show() ;
        }

    }) ;
    st_del.events().click([&](){
        if(!st_list.selected().empty()) {
            auto kp = st_list.selected().front() ;
            opt_name = st_list.at(kp).text(0) ;
            SQLite::Transaction tra(db) ;
            SQLite::Statement del_stat(db, "Delete from students where name = ?") ;
            del_stat.bind(1, opt_name) ;
            del_stat.executeStep() ;
            tra.commit() ;
            new_st = false ;
            st_list.erase(st_list.at(kp)) ;
        }

    }) ;
    st_save.events().click([&](){
        auto name = st_name.text() ;
        auto gender = st_gender.text() ;
        auto birthday = st_birthday.text() ;
        auto hobby = st_hobby.text() ;
        auto mb = st_mb.text() ;
        auto qq = st_qq.text() ;
        if(new_st) {
            SQLite::Transaction tra(db) ;
            SQLite::Statement add_stat(db, "Insert Into students (name, gender, birthday, hobby, mb, qq) values (?, ?, ?, ?, ?, ?)") ;
            add_stat.bind(1, name) ;
            add_stat.bind(2, gender) ;
            add_stat.bind(3, birthday) ;
            add_stat.bind(4, hobby) ;
            add_stat.bind(5, mb) ;
            add_stat.bind(6, qq) ;
            add_stat.executeStep() ;
            tra.commit() ;

            cat.append(name) ;        
        }else {
            SQLite::Transaction tra(db) ;
            SQLite::Statement add_stat(db, "Update students Set name = ?, gender = ?, birthday = ?, hobby = ?, mb = ?, qq = ? where name = ?") ;
            add_stat.bind(1, name) ;
            add_stat.bind(2, gender) ;
            add_stat.bind(3, birthday) ;
            add_stat.bind(4, hobby) ;
            add_stat.bind(5, mb) ;
            add_stat.bind(6, qq) ;
            add_stat.bind(7, opt_name) ;
            add_stat.executeStep() ;
            tra.commit() ;
        }
        view_form.hide() ;
    }) ;

    st_cancel.events().click([&](){
        view_form.hide() ;
    }) ;

    nana::exec() ;
    return 0;
}
