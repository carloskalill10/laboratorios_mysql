module.exports = (sequelize, DataType) => {
    const Reserves = sequelize.define("Reserves", {
        id: {
            type: DataType.INTEGER,
            primaryKey: true,
            autoIncrement: true
        },
        dt_entrada: {
            type: DataType.DATE,
            allowNull: false,
            validate: {
                notEmpty: true
            }

        },
        dt_saida: {
            type: DataType.DATE,
            allowNull: false,
            validate: {
                notEmpty: true
            }

        }
    }, {
            classMethods: {
                associate: (models) => {
                    Reserves.belongsTo(models.Users);
                    Reserves.belongsTo(models.Laboratories);
                }
            }
        });
    return Reserves;
};
