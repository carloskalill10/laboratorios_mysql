module.exports = app => {
    const Laboratories = app.db.models.Laboratories;

    const controllerLaboratory = {
        createLaboratory: (req, res) => {
            Laboratories.create(req.body)
                .then(result => res.json(result))
                .catch(error => {
                    res.status(412).json({ error: error.message });
                });

        },
        findAllLaboratory: (req, res) => {
            Laboratories.findAll({ where: { ativo: true } })
                .then(result => res.json(result))/*remover result do json e por uma msg de ok*/
                .catch(error => {
                    res.status(412).json({ msg: error.message });
                });

        },
        update:(req,res)=>{
            Laboratories.update(req.body, { where: req.params })
            .then(result => res.sendStatus(204))
            .catch(error => {
                res.status(412).json({ msg: error.message });
            })

        },
        disable:(req,res)=>{
            let ativo = {'ativo':0}
            Laboratories.update(ativo, { where: req.params })
            .then(result => res.sendStatus(204))
            .catch(error => {
                res.status(412).json({ msg: error.message });
            })
        }
    };

    return controllerLaboratory;


}