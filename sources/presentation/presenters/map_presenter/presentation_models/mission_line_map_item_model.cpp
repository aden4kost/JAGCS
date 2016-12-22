#include "mission_line_map_item_model.h"

// Qt
#include <QGeoCoordinate>
#include <QDebug>

// Internal
#include "mission.h"
#include "position_mission_item.h"

using namespace presentation;

MissionLineMapItemModel::MissionLineMapItemModel(QObject* parent):
    QAbstractListModel(parent)
{}

int MissionLineMapItemModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_missions.count();
}

QVariant MissionLineMapItemModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= m_missions.count()) return QVariant();

    domain::Mission* mission = m_missions.at(index.row());

    switch (role)
    {
    case MissionPathRole:
    {
        QVariantList line;
        for (domain::MissionItem* item: mission->items())
        {
            domain::PositionMissionItem* positionItem =
                    qobject_cast<domain::PositionMissionItem*>(item);
            if (positionItem)
            {
                QGeoCoordinate coordinate(positionItem->latitude(),
                                          positionItem->longitude());
                if (coordinate.isValid()) line.append(
                            QVariant::fromValue(coordinate));
            }
        }
        return line;
    }
    default:
        return QVariant();
    }
}

void MissionLineMapItemModel::addMission(domain::Mission* mission)
{
    this->beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());

    m_missions.append(mission);

    connect(mission, &domain::Mission::missionItemAdded,
            this, &MissionLineMapItemModel::onMissionItemAdded);
    connect(mission, &domain::Mission::missionItemRemoved,
            this, &MissionLineMapItemModel::onMissionItemRemoved);

    for (domain::MissionItem* item: mission->items())
        this->onMissionItemAdded(item);

    this->endInsertRows();
}

void MissionLineMapItemModel::removeMission(domain::Mission* mission)
{
    int row = m_missions.indexOf(mission);
    if (row == -1) return;

    this->beginRemoveRows(QModelIndex(), row, row);
    m_missions.removeOne(mission);

    this->endRemoveRows();
}

QHash<int, QByteArray> MissionLineMapItemModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[MissionPathRole] = "missionPath";

    return roles;
}

QModelIndex MissionLineMapItemModel::missionIndex(domain::Mission* mission) const
{
    return this->index(m_missions.indexOf(mission));
}

void MissionLineMapItemModel::onMissionItemAdded(domain::MissionItem* item)
{
    domain::Mission* mission = item->mission();

    domain::PositionMissionItem* positionItem =
            qobject_cast<domain::PositionMissionItem*>(item);
    if (positionItem)
    {
        connect(positionItem, &domain::PositionMissionItem::latitudeChanged,
                this, [this, mission] { this->updateMissionPath(mission); });
        connect(positionItem, &domain::PositionMissionItem::longitudeChanged,
                this, [this, mission] { this->updateMissionPath(mission); });
    }

    this->updateMissionPath(mission);
}

void MissionLineMapItemModel::onMissionItemRemoved(domain::MissionItem* item)
{
    disconnect(item, nullptr, this, nullptr);

    this->updateMissionPath(qobject_cast<domain::Mission*>(this->sender()));
}

void MissionLineMapItemModel::updateMissionPath(domain::Mission* mission)
{
    QModelIndex index = this->missionIndex(mission);
    if (index.isValid()) emit dataChanged(index, index, { MissionPathRole });
}
