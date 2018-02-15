module.exports = app => {
    const Users = app.db.models.Users;

    const controllerUser = {
        createUser: (req, res) => {
            Users.create(req.body)
                .then(result => res.json(result))
                .catch(error => {
                    res.status(412).json({ error: error.message });
                });

        },
        findAllUser: (req, res) => {
            Users.findAll({ where: { ativo: true } })
                .then(result => res.json(result))/*remover result do json e por uma msg de ok*/
                .catch(error => {
                    res.status(412).json({ error: error.message });
                });

        },
        update:(req,res)=>{
            Users.update(req.body, { where: req.params })
            .then(result => res.sendStatus(204))
            .catch(error => {
                res.status(412).json({ error: error.message });
            })

        },
        disable:(req,res)=>{
            let ativo = {'ativo':0}
            Users.update(ativo, { where: req.params })
            .then(result => res.sendStatus(204))
            .catch(error => {
                res.status(412).json({ error: error.message });
            })
        }
    };

    return controllerUser;


}