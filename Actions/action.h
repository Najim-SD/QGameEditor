#ifndef ACTION_H
#define ACTION_H

#include <QObject>

enum ActionType { Change_Animation, Change_Animation_Direction, Change_Cursor, Change_Parent,
                  Change_Path, Change_Transparency, Change_Z_Depth, Collision_State, Conditional_Action,
                  Create_Actor, Create_Timer, Destroy_Actor, Destroy_Timer, Event_Disable, Event_Enable,
                  Follow_Mouse, Move_To, Physical_Response, Play_Music, Play_Sound, Script_Editor, Set_Text,
                  To_Anterior_Position, Visibility_State };




static const QStringList actionTypeString { "Change Animation", "Change Animation Direction", "Change Cursor", "Change Parent",
            "Change Path", "Change Transparency", "Change Z Depth", "Collision State", "Conditional Action",
            "Create Actor", "Create Timer", "Destroy Actor", "Destroy Timer", "Event Disable", "Event Enable",
            "Follow Mouse", "Move To", "Physical Response", "Play Music", "Play Sound", "Script Editor", "Set Text",
            "To Anterior Position", "Visibility State" };

class Action : public QObject
{
    Q_OBJECT
public:
    Action();

    ActionType type{Change_Animation};
    QString name;
    QString description;

signals:

public slots:
};

#endif // ACTION_H
