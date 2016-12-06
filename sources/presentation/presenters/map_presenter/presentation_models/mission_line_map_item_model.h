#ifndef MISSION_LINE_MAP_ITEM_MODEL_H
#define MISSION_LINE_MAP_ITEM_MODEL_H

#include <QAbstractListModel>

namespace domain
{
    class Mission;
}

namespace presentation
{
    class MissionLineMapItemModel: public QAbstractListModel
    {
        Q_OBJECT

    public:
        enum MissionLineMapItemRoles
        {
            MissionPathRole = Qt::UserRole + 1
        };

        MissionLineMapItemModel(QObject* parent = nullptr);

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role) const override;

    public slots:
        void addMission(domain::Mission* mission);
        void removeMission(domain::Mission* mission);

    protected:
        QHash<int, QByteArray> roleNames() const override;

        QModelIndex missionIndex(domain::Mission* mission) const;

    private slots:
        void onMissionItemsCountChanged();

    private:
        QList<domain::Mission*> m_missions;
    };
}

#endif // MISSION_LINE_MAP_ITEM_MODEL_H
