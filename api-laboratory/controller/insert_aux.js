module.exports = app => {
    const Insert_aux = app.db.models.Insert_aux;
    const sequelize = app.db.sequelize;

    const controllerInsert_aux = {
        createInsert_aux: (req, res) => {
            Insert_aux.create(req.body.insert_aux)
                .then(result => res.json({ok:"ok"}))
                .catch(error => {
                    res.status(412).json({ ok:"nop" });
                });

        },
        findIntevalOneMinuteLater: (req, res) => {
            sequelize.query('select * from Insert_auxes where created_at > NOW() - INTERVAL 1 MINUTE order by created_at desc limit 1',
                { type: sequelize.QueryTypes.SELECT })
                .then(result => res.json(result))
                .catch(error => {
                    res.status(412).json({ error: error.message });
                });
        }
    };

    return controllerInsert_aux;


}