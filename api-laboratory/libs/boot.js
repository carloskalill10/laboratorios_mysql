module.exports=app=>{
    app.db.sequelize.sync().done(()=>{
        app.listen(app.get("port"),()=>{
            console.log('TCC ifam v2 Rodando:'+ app.get("port"));
        });
    });
};
