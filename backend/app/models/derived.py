from sqlmodel import Field, Relationship, SQLModel
from typing import List, Optional
from datetime import datetime

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from app.models.session import Lap
    from app.models.reference import Corner

class CornerPerformance(SQLModel, table=True):
    __tablename__ = "corner_performances" # type: ignore

    performance_id: int             = Field(primary_key=True)
    entry_speed_kmh: float | None   = Field(default=None)
    time_in_corner_ms: int | None   = Field(default=None)

    lap_id: int | None = Field(
        default=None,
        foreign_key="laps.lap_id"
    )

    corner_id: int | None = Field(
        default=None,
        foreign_key="corners.corner_id"
    )

    lap: "Lap"          = Relationship(back_populates="corner_performances")
    corner: "Corner"    = Relationship(back_populates="corner_performances")


class Event(SQLModel, table=True):
    __tablename__ = "events" # type: ignore

    event_id: int           = Field(primary_key=True)
    event_type: str | None  = Field(default=None)
    severity: int | None    = Field(default=None)

    lap_id: int | None = Field(
        default=None,
        foreign_key="laps.lap_id"
    )

    corner_id: int | None = Field(
        default=None,
        foreign_key="corners.corner_id"
    )

    lap: "Lap"                  = Relationship(back_populates="events")
    corner: "Corner"            = Relationship(back_populates="events")
    video_clip: "VideoClip"     = Relationship(back_populates="event")


class VideoClip(SQLModel, table=True):
    __tablename__ = "video_clips" # type: ignore

    clip_id: int            = Field(primary_key=True)
    file_path: str | None   = Field(default=None)

    event_id: int | None = Field(
        default=None,
        foreign_key="events.event_id",
        unique=True
    )

    event: Optional["Event"] = Relationship(back_populates="video_clip")
