module.exports = (sequelize, DataType) => {
    const Acess = sequelize.define("Acess", {
        id: {
            type: DataType.INTEGER,
            primaryKey: true,
            autoIncrement: true
        },
        dt_acesso: {
            type: DataType.DATE,
            allowNull: false,
            defaultValue: DataType.NOW,
            validate: {
                notEmpty: true
            }

        }
    }, {
            classMethods: {
                associate: (models) => {
                    Acess.belongsTo(models.Users);
                    Acess.belongsTo(models.Laboratories);
                }
            }
        });
    return Acess;
};
