module.exports = app => {
    const Reserves = app.db.models.Reserves;
    const sequelize = app.db.sequelize;
    const controllerReserve = {
        createReserve: (req, res) => {
            Reserves.create(req.body)
                .then(result => res.json(result))
                .catch(error => {
                    res.status(412).json({ error: error.message });
                });

        },
        delete:(req,res)=>{
            Reserves.destroy({ where: req.params })
            .then(result => res.sendStatus(204))
            .catch(error => {
                res.status(412).json({ error: error.message });
            })
        },
        findAcessForDateAndLab: (req, res) => {
            const query = req.params;
            sequelize.query(`SELECT Reserves.id,Users.nome,Users.tag,Laboratories.nome as lab, date_format(Reserves.created_at,'%d/%m/%Y %H:%i:%s') as created_at,date_format(Reserves.dt_entrada,'%d/%m/%Y %H:%i:%s') as dt_entrada,date_format(Reserves.dt_saida,'%d/%m/%Y %H:%i:%s') as dt_saida FROM Reserves 
                    INNER JOIN Users ON Users.id = Reserves.user_id
                    INNER JOIN Laboratories ON Laboratories.id = Reserves.laboratory_id
                    WHERE Reserves.dt_entrada BETWEEN "${query.date} 00:00:00" AND "${query.date} 23:59:59" AND Reserves.laboratory_id = ${query.lab}`,
                { type: sequelize.QueryTypes.SELECT })
                .then(result => res.json(result))
                .catch(error => {
                    res.status(412).json({ error: error.message });
                });

        }
    };

    return controllerReserve;


}