import QtQuick 2.6
import QtLocation 5.6
import QtPositioning 5.6

import "Overlays"

Map {
    id: root

    property var lineModel
    property var pointModel
    property var vehicleModel

    property bool vehicleVisible: true
    property bool missionPointsVisible: true
    property bool missionLinesVisible: true
    property bool trackVisible: true
    property bool hdopVisible: true
    property bool homeVisible: true

    property var mouseCoordinate: QtPositioning.coordinate()

    signal picked(var coordinate)

    implicitHeight: width

    plugin: Plugin {
        name: "osm"

        PluginParameter {
            name: "osm.mapping.custom.host";
            value: settings.value("Map/tileHost", "http://a.tile.openstreetmap.org/");
        }

        PluginParameter {
            name: "osm.mapping.offline.directory";
            value: settings.value("Map/cacheFolder", "~/.cache/QtLocation/osm");
        }

        PluginParameter {
            name: "osm.mapping.cache.disk.size";
            value: settings.value("Map/cacheSize", 52428800);
        }

        PluginParameter {
            name: "osm.mapping.providersrepository.disabled";
            value: true
        }
    }
    gesture.flickDeceleration: 3000
    gesture.enabled: true
    copyrightsVisible: false

    Behavior on center {
        CoordinateAnimation {
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }

    MissionLineMapOverlayView {
        model: missionLinesVisible ? lineModel : 0
    }

    RadiusMapOverlayView {
        model: missionPointsVisible ? pointModel : 0
    }

    AcceptanceRadiusMapOverlayView {
        model: missionPointsVisible ? pointModel : 0
    }

    MissionPointMapOverlayView {
        model: missionPointsVisible ? pointModel : 0
    }

    HomeMapOverlayView {
        model: homeVisible ? vehicleModel : 0
    }

    VehicleMapOverlayView {
        model: vehicleVisible ? vehicleModel : 0
    }

    TrackMapOverlayView {
        model: trackVisible ? vehicleModel : 0
    }

    HdopRadiusMapOverlayView {
        model: hdopVisible ? vehicleModel : 0
    }

//    MapStatusBar {
//        id: bar
//        anchors.bottom: parent.bottom
//        coordinate: root.mouseCoordinate
//        width: parent.width
//        z: 3
//    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onExited: mouseCoordinate = QtPositioning.coordinate()
        onPositionChanged: mouseCoordinate = root.toCoordinate(Qt.point(mouseX, mouseY))
        onClicked: root.picked(mouseCoordinate)
    }

    Component.onCompleted: {
        center = QtPositioning.coordinate(settings.value("Map/centerLatitude"),
                                          settings.value("Map/centerLongitude"));
        zoomLevel = settings.value("Map/zoomLevel");
    }

    Component.onDestruction: {
        settings.setValue("Map/centerLatitude", center.latitude);
        settings.setValue("Map/centerLongitude", center.longitude);
        settings.setValue("Map/zoomLevel", zoomLevel);
    }

    onCenterChanged: {
        if (!mouseArea.containsMouse) return;

        mouseCoordinate = root.toCoordinate(Qt.point(mouseArea.mouseX, mouseArea.mouseY))
    }

    function setGesturesEnabled(enabled) {
        gesture.acceptedGestures = enabled ?
                    (MapGestureArea.PinchGesture |
                     MapGestureArea.PanGesture |
                     MapGestureArea.FlickGesture) :
                    MapGestureArea.PinchGesture
    }
}
