module.exports = (sequelize, DataType) => {
    const Users = sequelize.define("Users", {
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
        tag: {
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
        },
        tipo: {
            type: DataType.INTEGER,
            allowNull: false,
            defaultValue: 0,//0 user comum 1- admin
        },
        funcao: {
            type: DataType.INTEGER,
            allowNull: false,
            defaultValue: 0,//0 PROFESSOR 1- ALUNO 2-ADMINISTRATIVO 3- TECNICO 4 OUTRA
        },
        login: {
            type: DataType.STRING,
            allowNull: false,
            validate: {
                notEmpty: true
            }
        },
        senha: {
            type: DataType.STRING,
            allowNull: false,
            validate: {
                notEmpty: true
            }
        }
    }, {
            classMethods: {
                associate: (models) => {
                    Users.hasMany(models.Reserves);
                    Users.hasMany(models.Acess);
                }
            }
        });
    return Users;
};
