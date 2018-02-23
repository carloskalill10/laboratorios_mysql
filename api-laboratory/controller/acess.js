module.exports = app => {
    const Acess = app.db.models.Acess;
    const sequelize = app.db.sequelize;

    const controllerAcess = {
        verifyAndCreateAcess: (req, res) => {
            const acessR = req.body.acess;
            sequelize.query(`SELECT Reserves.user_id,Reserves.laboratory_id FROM Reserves
                INNER JOIN Users ON Users.id = Reserves.user_id
                INNER JOIN Laboratories ON Laboratories.id = Reserves.laboratory_id
                WHERE Users.tag="${acessR.tag}" AND laboratory_id="${acessR.lab_id}" AND NOW() between Reserves.dt_entrada and Reserves.dt_saida limit 1`, { type: sequelize.QueryTypes.SELECT })
                .then(result => {
                    if (result.length!=0) {
                        const acessSave = {'user_id': result[0].user_id, 'laboratory_id': result[0].laboratory_id };
                        Acess.create(acessSave)
                            .then(result => res.json({"ok":"ok"}))
                            .catch(error => {
                                res.status(412).json({"ok":"nop"});
                            });
                    }else{
                        res.status(412).json({"ok":"nop"});
                    }

                })
                .catch(error => {
                    res.status(412).json({"ok":"nop"});
                });

        },
        findAcessForDateAndLab: (req, res) => {
            const query = req.params;
            sequelize.query(`SELECT Users.nome,Users.tag,Laboratories.nome as lab, date_format (Acesses.created_at,'%d/%m/%Y %H:%i:%s') as created_at FROM Acesses
                                INNER JOIN Users ON Users.id = Acesses.user_id
                                INNER JOIN Laboratories ON Laboratories.id = Acesses.laboratory_id
                                WHERE Acesses.created_at BETWEEN "${query.date} 00:00:00" AND "${query.date} 23:59:59" AND Acesses.laboratory_id = ${query.lab}`,
                { type: sequelize.QueryTypes.SELECT })
                .then(result => res.json(result))
                .catch(error => {
                    res.status(412).json({ error: error.message });
                });

        }
    };


    return controllerAcess;


}
