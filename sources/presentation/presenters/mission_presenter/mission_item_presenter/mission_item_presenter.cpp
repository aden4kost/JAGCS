#include "mission_item_presenter.h"

// Qt
#include <QMap>
#include <QVariant>
#include <QDebug>

// Internal
#include "mission_item.h"

#include "mission_service.h"

using namespace presentation;

class MissionItemPresenter::Impl
{
public:
    db::MissionPtr selectedMission;
    db::MissionItemPtr item;

    domain::MissionService* service;

    const QMap<db::MissionItem::Command, QString> commands = {
        { db::MissionItem::UnknownCommand, tr("None") },
        { db::MissionItem::Takeoff, tr("Takeoff") },
        { db::MissionItem::Waypoint, tr("Waypoint") },
        { db::MissionItem::LoiterAltitude, tr("LoiterAltitude") },
        { db::MissionItem::LoiterTurns, tr("LoiterTurns") },
        { db::MissionItem::Continue, tr("Continue") },
        { db::MissionItem::Return, tr("Return") },
        { db::MissionItem::Landing, tr("Landing") } };
};

MissionItemPresenter::MissionItemPresenter(domain::MissionService* service,
                                           QObject* object):
    BasePresenter(object),
    d(new Impl())
{
    d->service = service;

    connect(service, &domain::MissionService::missionItemAdded,
            this, &MissionItemPresenter::updateCount);
    connect(service, &domain::MissionService::missionItemRemoved,
            this, &MissionItemPresenter::updateCount);
    connect(service, &domain::MissionService::missionItemChanged,
            this, [this](const db::MissionItemPtr& item) {
        if (item == d->item) this->updateItem();
    });
}

MissionItemPresenter::~MissionItemPresenter()
{
    if (d->selectedMission)
    {
        d->service->saveMissionItems(d->selectedMission);
    }
}

db::MissionPtr MissionItemPresenter::selectedMission() const
{
    return d->selectedMission;
}

void MissionItemPresenter::setMission(const db::MissionPtr& mission)
{
    if (d->selectedMission == mission) return;

    if (d->selectedMission)
    {
        d->service->saveMissionItems(d->selectedMission);
    }

    d->selectedMission = mission;
    this->updateCount(true);
}

void MissionItemPresenter::setMissionItem(const db::MissionItemPtr& item)
{
    if (d->item == item) return;

    d->item = item;
    this->updateItem();
}

void MissionItemPresenter::connectView(QObject* view)
{
    connect(view, SIGNAL(addItem()), this, SLOT(onAddItem()));
    connect(view, SIGNAL(removeItem()), this, SLOT(onRemoveItem()));
    connect(view, SIGNAL(selectItem(int)), this, SLOT(onSelectItem(int)));

    connect(view, SIGNAL(setCommand(int)), this, SLOT(onSetCommand(int)));
    connect(view, SIGNAL(setAltitude(qreal)), this, SLOT(onSetAltitude(qreal)));
    connect(view, SIGNAL(setAltitudeRelative(bool)), this, SLOT(onSetAltitudeRelative(bool)));
    connect(view, SIGNAL(setLatitude(qreal)), this, SLOT(onSetLatitude(qreal)));
    connect(view, SIGNAL(setLongitude(qreal)), this, SLOT(onSetLongitude(qreal)));
    connect(view, SIGNAL(setRadius(qreal)), this, SLOT(onSetRadius(qreal)));
    connect(view, SIGNAL(setPeriods(int)), this, SLOT(onSetPeriods(int)));
    connect(view, SIGNAL(setPitch(qreal)), this, SLOT(onSetPitch(qreal)));

    QStringList commands = d->commands.values();
    this->setViewProperty(PROPERTY(commands), QVariant::fromValue(commands));

    this->updateCount(true);
}

void MissionItemPresenter::updateCount(bool gotoNewItem)
{
    if (d->selectedMission)
    {
        db::MissionItemPtrList items = d->service->missionItems(d->selectedMission);

        this->setViewProperty(PROPERTY(count), items.count());
        if (gotoNewItem) this->onSelectItem(items.count());
    }
    else
    {
        this->setViewProperty(PROPERTY(count), 0);
        d->item.clear();
        this->updateItem();
    }
}

void MissionItemPresenter::updateItem()
{
    if (d->item)
    {
        this->setViewProperty(PROPERTY(sequence), d->item->sequence());
        this->setViewProperty(PROPERTY(command), d->item->command());
        this->setViewProperty(PROPERTY(altitude), d->item->altitude());
        this->setViewProperty(PROPERTY(isAltitudeRelative), d->item->isAltitudeRelative());
        this->setViewProperty(PROPERTY(latitude), d->item->latitude());
        this->setViewProperty(PROPERTY(longitude), d->item->longitude());
        this->setViewProperty(PROPERTY(radius), d->item->radius());
        this->setViewProperty(PROPERTY(pitch), d->item->pitch());
        this->setViewProperty(PROPERTY(periods), d->item->periods());
    }
    else
    {
        this->setViewProperty(PROPERTY(sequence), 0);
        this->setViewProperty(PROPERTY(command), db::MissionItem::UnknownCommand);
    }
}

void MissionItemPresenter::onAddItem()
{
    if (d->selectedMission.isNull()) return;

    d->service->addNewMissionItem(d->selectedMission);
    this->onSelectItem(this->viewProperty(PROPERTY(count)).toInt());
}

void MissionItemPresenter::onRemoveItem()
{
    if (d->item.isNull()) return;

    d->service->removeMissionItem(d->item);
}

void MissionItemPresenter::onSelectItem(int index)
{
    if (d->selectedMission.isNull()) return;

    d->item = d->service->missionItem(d->selectedMission, index);
    this->updateItem();
}

void MissionItemPresenter::onSetCommand(int command)
{
    if (d->item.isNull()) return;

    d->item->setCommand(db::MissionItem::Command(command));
    this->updateItem();
}

void MissionItemPresenter::onSetAltitude(qreal altitude)
{
    if (d->item.isNull()) return;

    d->item->setAltitude(altitude);
}

void MissionItemPresenter::onSetAltitudeRelative(bool relative)
{
    if (d->item.isNull()) return;

    d->item->setAltitudeRelative(relative);
}

void MissionItemPresenter::onSetLatitude(qreal latitude)
{
    if (d->item.isNull()) return;

    d->item->setLatitude(latitude);
}

void MissionItemPresenter::onSetLongitude(qreal longitude)
{
    if (d->item.isNull()) return;

    d->item->setLongitude(longitude);
}

void MissionItemPresenter::onSetRadius(qreal radius)
{
    if (d->item.isNull()) return;

    d->item->setRadius(radius);
}

void MissionItemPresenter::onSetPeriods(int periods)
{
    if (d->item.isNull()) return;

    d->item->setPeriods(periods);
}

void MissionItemPresenter::onSetPitch(qreal pitch)
{
    if (d->item.isNull()) return;

    d->item->setPitch(pitch);
}
