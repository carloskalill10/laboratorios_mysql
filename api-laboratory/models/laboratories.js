module.exports = (sequelize, DataType) => {
    const Laboratories = sequelize.define("Laboratories", {
        id: {
            type: DataType.INTEGER,
            primaryKey: true,
            autoIncrement: true
        },
        nome: {
            type: DataType.STRING,
            allowNull: false,
            validate: {
                notEmpty: true
            }
        },
        ativo: {
            type: DataType.BOOLEAN,
            allowNull: false,
            defaultValue: true
        }
    }, {
            classMethods: {
                associate: (models) => {
                    Laboratories.hasMany(models.Reserves);
                    Laboratories.hasMany(models.Acess);
                }
            }
        });
    return Laboratories;
};
